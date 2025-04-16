static int ctnetlink_parse_help(const struct nlattr *attr, char **helper_name,
				struct nlattr **helpinfo)
{
	int err;
	struct nlattr *tb[CTA_HELP_MAX+1];

	err = nla_parse_nested_deprecated(tb, CTA_HELP_MAX, attr,
					  help_nla_policy, NULL);
	if (err < 0)
		return err;

	if (!tb[CTA_HELP_NAME])
		return -EINVAL;

	*helper_name = nla_data(tb[CTA_HELP_NAME]);

	if (tb[CTA_HELP_INFO])
		*helpinfo = tb[CTA_HELP_INFO];

	return 0;
}