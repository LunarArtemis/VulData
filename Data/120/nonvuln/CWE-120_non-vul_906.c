static int validate_range(struct v4l2_subdev *sd,
					struct v4l2_ext_control *control)
{
	struct v4l2_queryctrl vqc;
	int rval;

	vqc.id = control->id;
	rval = si4713_queryctrl(sd, &vqc);
	if (rval < 0)
		goto exit;

	if (control->value < vqc.minimum || control->value > vqc.maximum)
		rval = -ERANGE;

exit:
	return rval;
}