FLAC_API FLAC__bool FLAC__stream_encoder_process_interleaved(FLAC__StreamEncoder *encoder, const FLAC__int32 buffer[], uint32_t samples)
{
	uint32_t i, j, k, channel;
	FLAC__int32 x, mid, side;
	const uint32_t channels = encoder->protected_->channels, blocksize = encoder->protected_->blocksize;

	FLAC__ASSERT(0 != encoder);
	FLAC__ASSERT(0 != encoder->private_);
	FLAC__ASSERT(0 != encoder->protected_);
	FLAC__ASSERT(encoder->protected_->state == FLAC__STREAM_ENCODER_OK);

	j = k = 0;
	/*
	 * we have several flavors of the same basic loop, optimized for
	 * different conditions:
	 */
	if(encoder->protected_->do_mid_side_stereo && channels == 2) {
		/*
		 * stereo coding: unroll channel loop
		 */
		do {
			if(encoder->protected_->verify)
				append_to_verify_fifo_interleaved_(&encoder->private_->verify.input_fifo, buffer, j, channels, flac_min(blocksize+OVERREAD_-encoder->private_->current_sample_number, samples-j));

			/* "i <= blocksize" to overread 1 sample; see comment in OVERREAD_ decl */
			for(i = encoder->private_->current_sample_number; i <= blocksize && j < samples; i++, j++) {
				encoder->private_->integer_signal[0][i] = mid = side = buffer[k++];
				x = buffer[k++];
				encoder->private_->integer_signal[1][i] = x;
				mid += x;
				side -= x;
				mid >>= 1; /* NOTE: not the same as 'mid = (left + right) / 2' ! */
				encoder->private_->integer_signal_mid_side[1][i] = side;
				encoder->private_->integer_signal_mid_side[0][i] = mid;
			}
			encoder->private_->current_sample_number = i;
			/* we only process if we have a full block + 1 extra sample; final block is always handled by FLAC__stream_encoder_finish() */
			if(i > blocksize) {
				if(!process_frame_(encoder, /*is_fractional_block=*/false, /*is_last_block=*/false))
					return false;
				/* move unprocessed overread samples to beginnings of arrays */
				FLAC__ASSERT(i == blocksize+OVERREAD_);
				FLAC__ASSERT(OVERREAD_ == 1); /* assert we only overread 1 sample which simplifies the rest of the code below */
				encoder->private_->integer_signal[0][0] = encoder->private_->integer_signal[0][blocksize];
				encoder->private_->integer_signal[1][0] = encoder->private_->integer_signal[1][blocksize];
				encoder->private_->integer_signal_mid_side[0][0] = encoder->private_->integer_signal_mid_side[0][blocksize];
				encoder->private_->integer_signal_mid_side[1][0] = encoder->private_->integer_signal_mid_side[1][blocksize];
				encoder->private_->current_sample_number = 1;
			}
		} while(j < samples);
	}
	else {
		/*
		 * independent channel coding: buffer each channel in inner loop
		 */
		do {
			if(encoder->protected_->verify)
				append_to_verify_fifo_interleaved_(&encoder->private_->verify.input_fifo, buffer, j, channels, flac_min(blocksize+OVERREAD_-encoder->private_->current_sample_number, samples-j));

			/* "i <= blocksize" to overread 1 sample; see comment in OVERREAD_ decl */
			for(i = encoder->private_->current_sample_number; i <= blocksize && j < samples; i++, j++) {
				for(channel = 0; channel < channels; channel++)
					encoder->private_->integer_signal[channel][i] = buffer[k++];
			}
			encoder->private_->current_sample_number = i;
			/* we only process if we have a full block + 1 extra sample; final block is always handled by FLAC__stream_encoder_finish() */
			if(i > blocksize) {
				if(!process_frame_(encoder, /*is_fractional_block=*/false, /*is_last_block=*/false))
					return false;
				/* move unprocessed overread samples to beginnings of arrays */
				FLAC__ASSERT(i == blocksize+OVERREAD_);
				FLAC__ASSERT(OVERREAD_ == 1); /* assert we only overread 1 sample which simplifies the rest of the code below */
				for(channel = 0; channel < channels; channel++)
					encoder->private_->integer_signal[channel][0] = encoder->private_->integer_signal[channel][blocksize];
				encoder->private_->current_sample_number = 1;
			}
		} while(j < samples);
	}

	return true;
}