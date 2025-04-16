static int
match_selector(fz_css_selector *sel, fz_xml *node)
{
	if (!node)
		return 0;

	if (sel->combine)
	{
		/* descendant */
		if (sel->combine == ' ')
		{
			fz_xml *parent = fz_xml_up(node);
			while (parent)
			{
				if (match_selector(sel->left, parent))
					if (match_selector(sel->right, node))
						return 1;
				parent = fz_xml_up(parent);
			}
			return 0;
		}

		/* child */
		if (sel->combine == '>')
		{
			fz_xml *parent = fz_xml_up(node);
			if (!parent)
				return 0;
			if (!match_selector(sel->left, parent))
				return 0;
			if (!match_selector(sel->right, node))
				return 0;
		}

		/* adjacent */
		if (sel->combine == '+')
		{
			fz_xml *prev = fz_xml_prev(node);
			while (prev && !fz_xml_tag(prev))
				prev = fz_xml_prev(prev);
			if (!prev)
				return 0;
			if (!fz_xml_tag(prev))
				return 0;
			if (!match_selector(sel->left, prev))
				return 0;
			if (!match_selector(sel->right, node))
				return 0;
		}
	}

	if (sel->name)
	{
		if (strcmp(sel->name, fz_xml_tag(node)))
			return 0;
	}

	if (sel->cond)
	{
		if (!match_condition(sel->cond, node))
			return 0;
	}

	return 1;
}