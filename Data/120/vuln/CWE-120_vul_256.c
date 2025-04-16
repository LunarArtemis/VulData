bool ArgsParser::parse(int argc, char** argv, bool exit_on_failure)
{
    auto print_usage_and_exit = [this, argv, exit_on_failure] {
        print_usage(stderr, argv[0]);
        if (exit_on_failure)
            exit(1);
    };

    Vector<option> long_options;
    StringBuilder short_options_builder;

    int index_of_found_long_option = -1;

    // Tell getopt() to reset its internal state, and start scanning from optind = 1.
    // We could also set optreset = 1, but the host platform may not support that.
    optind = 0;

    for (size_t i = 0; i < m_options.size(); i++) {
        auto& opt = m_options[i];
        if (opt.long_name) {
            option long_opt {
                opt.long_name,
                opt.requires_argument ? required_argument : no_argument,
                &index_of_found_long_option,
                static_cast<int>(i)
            };
            long_options.append(long_opt);
        }
        if (opt.short_name) {
            short_options_builder.append(opt.short_name);
            if (opt.requires_argument)
                short_options_builder.append(':');
        }
    }
    long_options.append({ 0, 0, 0, 0 });

    String short_options = short_options_builder.build();

    while (true) {
        int c = getopt_long(argc, argv, short_options.characters(), long_options.data(), nullptr);
        if (c == -1) {
            // We have reached the end.
            break;
        } else if (c == '?') {
            // There was an error, and getopt() has already
            // printed its error message.
            print_usage_and_exit();
            return false;
        }

        // Let's see what option we just found.
        Option* found_option = nullptr;
        if (c == 0) {
            // It was a long option.
            VERIFY(index_of_found_long_option >= 0);
            found_option = &m_options[index_of_found_long_option];
            index_of_found_long_option = -1;
        } else {
            // It was a short option, look it up.
            auto it = m_options.find_if([c](auto& opt) { return c == opt.short_name; });
            VERIFY(!it.is_end());
            found_option = &*it;
        }
        VERIFY(found_option);

        const char* arg = found_option->requires_argument ? optarg : nullptr;
        if (!found_option->accept_value(arg)) {
            warnln("\033[31mInvalid value for option \033[1m{}\033[22m, dude\033[0m", found_option->name_for_display());
            print_usage_and_exit();
            return false;
        }
    }

    // We're done processing options, now let's parse positional arguments.

    int values_left = argc - optind;
    int num_values_for_arg[m_positional_args.size()];
    int total_values_required = 0;
    for (size_t i = 0; i < m_positional_args.size(); i++) {
        auto& arg = m_positional_args[i];
        num_values_for_arg[i] = arg.min_values;
        total_values_required += arg.min_values;
    }

    if (total_values_required > values_left) {
        print_usage_and_exit();
        return false;
    }
    int extra_values_to_distribute = values_left - total_values_required;

    for (size_t i = 0; i < m_positional_args.size(); i++) {
        auto& arg = m_positional_args[i];
        int extra_values_to_this_arg = min(arg.max_values - arg.min_values, extra_values_to_distribute);
        num_values_for_arg[i] += extra_values_to_this_arg;
        extra_values_to_distribute -= extra_values_to_this_arg;
        if (extra_values_to_distribute == 0)
            break;
    }

    if (extra_values_to_distribute > 0) {
        // We still have too many values :(
        print_usage_and_exit();
        return false;
    }

    for (size_t i = 0; i < m_positional_args.size(); i++) {
        auto& arg = m_positional_args[i];
        for (int j = 0; j < num_values_for_arg[i]; j++) {
            const char* value = argv[optind++];
            if (!arg.accept_value(value)) {
                warnln("Invalid value for argument {}", arg.name);
                print_usage_and_exit();
                return false;
            }
        }
    }

    // We're done parsing! :)
    // Now let's show help if requested.
    if (m_show_help) {
        print_usage(stdout, argv[0]);
        if (exit_on_failure)
            exit(0);
        return false;
    }

    return true;
}