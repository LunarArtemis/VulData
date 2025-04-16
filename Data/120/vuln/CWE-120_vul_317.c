int
Server_jack_init(Server *self)
{
    int i = 0;
    char client_name[32];
    char name[16];
    const char *server_name = "server";
    jack_options_t options = JackNullOption;
    jack_status_t status;
    int sampleRate = 0;
    int bufferSize = 0;
    int nchnls = 0;
    int total_nchnls = 0;
    int index = 0;
    int ret = 0;
    assert(self->audio_be_data == NULL);
    PyoJackBackendData *be_data = (PyoJackBackendData *) PyMem_RawMalloc(sizeof(PyoJackBackendData));
    self->audio_be_data = (void *) be_data;
    be_data->activated = 0;
    strncpy(client_name, self->serverName, 31);

    Py_BEGIN_ALLOW_THREADS
    be_data->midi_event_count = 0;

    if (self->duplex == 1)
    {
        be_data->jack_in_ports = (jack_port_t **) PyMem_RawCalloc(self->ichnls + self->input_offset, sizeof(jack_port_t *));
    }
    else
    {
        be_data->jack_in_ports = NULL;
    }

    be_data->jack_out_ports = (jack_port_t **) PyMem_RawCalloc(self->nchnls + self->output_offset, sizeof(jack_port_t *));
    be_data->jack_client = jack_client_open(client_name, options, &status, server_name);

    if (self->withJackMidi)
    {
        be_data->midi_events = (PyoJackMidiEvent *)PyMem_RawMalloc(512 * sizeof(PyoJackMidiEvent));

        for (i = 0; i < 512; i++)
        {
            be_data->midi_events[i].timestamp = -1;
        }
    }

    Py_END_ALLOW_THREADS

    if (be_data->jack_client == NULL)
    {
        Server_error(self, "Jack unable to create client.\n");

        if (status & JackServerFailed)
        {
            Server_debug(self, "Jack jack_client_open() failed, "
                         "status = 0x%2.0x\n", status);
        }

        return -1;
    }

    if (status & JackServerStarted)
    {
        Server_warning(self, "Jack server started.\n");
    }

    if (strcmp(self->serverName, jack_get_client_name(be_data->jack_client)) )
    {
        strcpy(self->serverName, jack_get_client_name(be_data->jack_client));
        Server_warning(self, "Jack name `%s' assigned.\n", self->serverName);
    }

    sampleRate = jack_get_sample_rate(be_data->jack_client);

    if (sampleRate != self->samplingRate)
    {
        self->samplingRate = (double)sampleRate;
        Server_warning(self, "Sample rate set to Jack engine sample rate: %" PRIu32 ".\n", sampleRate);
    }
    else
    {
        Server_debug(self, "Jack engine sample rate: %" PRIu32 ".\n", sampleRate);
    }

    if (sampleRate <= 0)
    {
        Server_error(self, "Jack invalid engine sample rate.");

        Py_BEGIN_ALLOW_THREADS
        jack_client_close(be_data->jack_client);
        Py_END_ALLOW_THREADS

        return -1;
    }

    bufferSize = jack_get_buffer_size(be_data->jack_client);

    if (bufferSize != self->bufferSize)
    {
        self->bufferSize = bufferSize;
        Server_warning(self, "Buffer size set to Jack engine buffer size: %" PRIu32 ".\n", bufferSize);
    }
    else
    {
        Server_debug(self, "Jack engine buffer size: %" PRIu32 ".\n", bufferSize);
    }

    if (self->withJackMidi)
    {
        Py_BEGIN_ALLOW_THREADS
        be_data->jack_midiin_port = jack_port_register(be_data->jack_client, "input",
                                    JACK_DEFAULT_MIDI_TYPE,
                                    JackPortIsInput, 0);
        be_data->jack_midiout_port = jack_port_register(be_data->jack_client, "output",
                                     JACK_DEFAULT_MIDI_TYPE,
                                     JackPortIsOutput, 0);
        Py_END_ALLOW_THREADS
    }

    if (self->duplex == 1)
    {
        nchnls = total_nchnls = self->ichnls + self->input_offset;

        while (nchnls-- > 0)
        {
            index = total_nchnls - nchnls - 1;
            ret = sprintf(name, "input_%i", index + 1);

            if (ret > 0)
            {

                Py_BEGIN_ALLOW_THREADS
                be_data->jack_in_ports[index] = jack_port_register(be_data->jack_client,
                                                name,
                                                JACK_DEFAULT_AUDIO_TYPE,
                                                JackPortIsInput, 0);
                Py_END_ALLOW_THREADS

            }

            if ((be_data->jack_in_ports[index] == NULL))
            {
                Server_error(self, "No more Jack input ports available.\n");
                return -1;
            }
        }
    }

    nchnls = total_nchnls = self->nchnls + self->output_offset;

    while (nchnls-- > 0)
    {
        index = total_nchnls - nchnls - 1;
        ret = sprintf(name, "output_%i", index + 1);

        if (ret > 0)
        {

            Py_BEGIN_ALLOW_THREADS
            be_data->jack_out_ports[index] = jack_port_register(be_data->jack_client,
                                             name,
                                             JACK_DEFAULT_AUDIO_TYPE,
                                             JackPortIsOutput, 0);
            Py_END_ALLOW_THREADS

        }

        if ((be_data->jack_out_ports[index] == NULL))
        {
            Server_error(self, "No more Jack output ports available.\n");
            return -1;
        }
    }

    jack_set_error_function(jack_error_cb);
    jack_set_sample_rate_callback(be_data->jack_client, jack_srate_cb, (void *) self);
    jack_on_shutdown(be_data->jack_client, jack_shutdown_cb, (void *) self);
    jack_set_buffer_size_callback(be_data->jack_client, jack_bufsize_cb, (void *) self);
    jack_set_process_callback(be_data->jack_client, jack_callback, (void *) self);

    if (self->isJackTransportSlave)
        jack_set_sync_callback(be_data->jack_client, jack_transport_cb, (void *) self);

    Py_BEGIN_ALLOW_THREADS
    ret = jack_activate(be_data->jack_client);
    Py_END_ALLOW_THREADS

    if (ret)
    {
        Server_error(self, "Jack cannot activate jack client.\n");
        return -1;
    }

    be_data->activated = 1;

    Server_jack_autoconnect(self);

    return 0;
}