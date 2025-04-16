static void
HTMLlineproc2body(Buffer *buf, Str (*feed) (), int llimit)
{
    static char *outc = NULL;
    static Lineprop *outp = NULL;
    static int out_size = 0;
    Anchor *a_href = NULL, *a_img = NULL, *a_form = NULL;
    char *p, *q, *r, *s, *t, *str;
    Lineprop mode, effect, ex_effect;
    int pos;
    int nlines;
#ifdef DEBUG
    FILE *debug = NULL;
#endif
    struct frameset *frameset_s[FRAMESTACK_SIZE];
    int frameset_sp = -1;
    union frameset_element *idFrame = NULL;
    char *id = NULL;
    int hseq, form_id;
    Str line;
    char *endp;
    char symbol = '\0';
    int internal = 0;
    Anchor **a_textarea = NULL;
#ifdef MENU_SELECT
    Anchor **a_select = NULL;
#endif
#if defined(USE_M17N) || defined(USE_IMAGE)
    ParsedURL *base = baseURL(buf);
#endif
#ifdef USE_M17N
    wc_ces name_charset = url_to_charset(NULL, &buf->currentURL,
					 buf->document_charset);
#endif

    if (out_size == 0) {
	out_size = LINELEN;
	outc = NewAtom_N(char, out_size);
	outp = NewAtom_N(Lineprop, out_size);
    }

    n_textarea = -1;
    if (!max_textarea) {	/* halfload */
	max_textarea = MAX_TEXTAREA;
	textarea_str = New_N(Str, max_textarea);
	a_textarea = New_N(Anchor *, max_textarea);
    }
#ifdef MENU_SELECT
    n_select = -1;
    if (!max_select) {		/* halfload */
	max_select = MAX_SELECT;
	select_option = New_N(FormSelectOption, max_select);
	a_select = New_N(Anchor *, max_select);
    }
#endif

#ifdef DEBUG
    if (w3m_debug)
	debug = fopen("zzzerr", "a");
#endif

    effect = 0;
    ex_effect = 0;
    nlines = 0;
    while ((line = feed()) != NULL) {
#ifdef DEBUG
	if (w3m_debug) {
	    Strfputs(line, debug);
	    fputc('\n', debug);
	}
#endif
	if (n_textarea >= 0 && *(line->ptr) != '<') {	/* halfload */
	    Strcat(textarea_str[n_textarea], line);
	    continue;
	}
      proc_again:
	if (++nlines == llimit)
	    break;
	pos = 0;
#ifdef ENABLE_REMOVE_TRAILINGSPACES
	Strremovetrailingspaces(line);
#endif
	str = line->ptr;
	endp = str + line->length;
	while (str < endp) {
	    PSIZE;
	    mode = get_mctype(str);
	    if ((effect | ex_efct(ex_effect)) & PC_SYMBOL && *str != '<') {
#ifdef USE_M17N
		char **buf = set_symbol(symbol_width0);
		int len;

		p = buf[(int)symbol];
		len = get_mclen(p);
		mode = get_mctype(p);
		PPUSH(mode | effect | ex_efct(ex_effect), *(p++));
		if (--len) {
		    mode = (mode & ~PC_WCHAR1) | PC_WCHAR2;
		    while (len--) {
			PSIZE;
			PPUSH(mode | effect | ex_efct(ex_effect), *(p++));
		    }
		}
#else
		PPUSH(PC_ASCII | effect | ex_efct(ex_effect), SYMBOL_BASE + symbol);
#endif
		str += symbol_width;
	    }
#ifdef USE_M17N
	    else if (mode == PC_CTRL || mode == PC_UNDEF) {
#else
	    else if (mode == PC_CTRL || IS_INTSPACE(*str)) {
#endif
		PPUSH(PC_ASCII | effect | ex_efct(ex_effect), ' ');
		str++;
	    }
#ifdef USE_M17N
	    else if (mode & PC_UNKNOWN) {
		PPUSH(PC_ASCII | effect | ex_efct(ex_effect), ' ');
		str += get_mclen(str);
	    }
#endif
	    else if (*str != '<' && *str != '&') {
#ifdef USE_M17N
		int len = get_mclen(str);
#endif
		PPUSH(mode | effect | ex_efct(ex_effect), *(str++));
#ifdef USE_M17N
		if (--len) {
		    mode = (mode & ~PC_WCHAR1) | PC_WCHAR2;
		    while (len--) {
			PSIZE;
			PPUSH(mode | effect | ex_efct(ex_effect), *(str++));
		    }
		}
#endif
	    }
	    else if (*str == '&') {
		/* 
		 * & escape processing
		 */
		p = getescapecmd(&str);
		while (*p) {
		    PSIZE;
		    mode = get_mctype((unsigned char *)p);
#ifdef USE_M17N
		    if (mode == PC_CTRL || mode == PC_UNDEF) {
#else
		    if (mode == PC_CTRL || IS_INTSPACE(*str)) {
#endif
			PPUSH(PC_ASCII | effect | ex_efct(ex_effect), ' ');
			p++;
		    }
#ifdef USE_M17N
		    else if (mode & PC_UNKNOWN) {
			PPUSH(PC_ASCII | effect | ex_efct(ex_effect), ' ');
			p += get_mclen(p);
		    }
#endif
		    else {
#ifdef USE_M17N
			int len = get_mclen(p);
#endif
			PPUSH(mode | effect | ex_efct(ex_effect), *(p++));
#ifdef USE_M17N
			if (--len) {
			    mode = (mode & ~PC_WCHAR1) | PC_WCHAR2;
			    while (len--) {
				PSIZE;
				PPUSH(mode | effect | ex_efct(ex_effect), *(p++));
			    }
			}
#endif
		    }
		}
	    }
	    else {
		/* tag processing */
		struct parsed_tag *tag;
		if (!(tag = parse_tag(&str, TRUE)))
		    continue;
		switch (tag->tagid) {
		case HTML_B:
		    effect |= PE_BOLD;
		    break;
		case HTML_N_B:
		    effect &= ~PE_BOLD;
		    break;
		case HTML_I:
		    ex_effect |= PE_EX_ITALIC;
		    break;
		case HTML_N_I:
		    ex_effect &= ~PE_EX_ITALIC;
		    break;
		case HTML_INS:
		    ex_effect |= PE_EX_INSERT;
		    break;
		case HTML_N_INS:
		    ex_effect &= ~PE_EX_INSERT;
		    break;
		case HTML_U:
		    effect |= PE_UNDER;
		    break;
		case HTML_N_U:
		    effect &= ~PE_UNDER;
		    break;
		case HTML_S:
		    ex_effect |= PE_EX_STRIKE;
		    break;
		case HTML_N_S:
		    ex_effect &= ~PE_EX_STRIKE;
		    break;
		case HTML_A:
		    if (renderFrameSet &&
			parsedtag_get_value(tag, ATTR_FRAMENAME, &p)) {
			p = url_quote_conv(p, buf->document_charset);
			if (!idFrame || strcmp(idFrame->body->name, p)) {
			    idFrame = search_frame(renderFrameSet, p);
			    if (idFrame && idFrame->body->attr != F_BODY)
				idFrame = NULL;
			}
		    }
		    p = r = s = NULL;
		    q = buf->baseTarget;
		    t = "";
		    hseq = 0;
		    id = NULL;
		    if (parsedtag_get_value(tag, ATTR_NAME, &id)) {
			id = url_quote_conv(id, name_charset);
			registerName(buf, id, currentLn(buf), pos);
		    }
		    if (parsedtag_get_value(tag, ATTR_HREF, &p))
			p = url_encode(remove_space(p), base,
				       buf->document_charset);
		    if (parsedtag_get_value(tag, ATTR_TARGET, &q))
			q = url_quote_conv(q, buf->document_charset);
		    if (parsedtag_get_value(tag, ATTR_REFERER, &r))
			r = url_encode(r, base,
				       buf->document_charset);
		    parsedtag_get_value(tag, ATTR_TITLE, &s);
		    parsedtag_get_value(tag, ATTR_ACCESSKEY, &t);
		    parsedtag_get_value(tag, ATTR_HSEQ, &hseq);
		    if (hseq > 0)
			buf->hmarklist =
			    putHmarker(buf->hmarklist, currentLn(buf),
				       pos, hseq - 1);
		    else if (hseq < 0) {
			int h = -hseq - 1;
			if (buf->hmarklist &&
			    h < buf->hmarklist->nmark &&
			    buf->hmarklist->marks[h].invalid) {
			    buf->hmarklist->marks[h].pos = pos;
			    buf->hmarklist->marks[h].line = currentLn(buf);
			    buf->hmarklist->marks[h].invalid = 0;
			    hseq = -hseq;
			}
		    }
		    if (id && idFrame)
			idFrame->body->nameList =
			    putAnchor(idFrame->body->nameList, id, NULL,
				      (Anchor **)NULL, NULL, NULL, '\0',
				      currentLn(buf), pos);
		    if (p) {
			effect |= PE_ANCHOR;
			a_href = registerHref(buf, p, q, r, s,
					      *t, currentLn(buf), pos);
			a_href->hseq = ((hseq > 0) ? hseq : -hseq) - 1;
			a_href->slave = (hseq > 0) ? FALSE : TRUE;
		    }
		    break;
		case HTML_N_A:
		    effect &= ~PE_ANCHOR;
		    if (a_href) {
			a_href->end.line = currentLn(buf);
			a_href->end.pos = pos;
			if (a_href->start.line == a_href->end.line &&
			    a_href->start.pos == a_href->end.pos) {
			    if (buf->hmarklist &&
				a_href->hseq < buf->hmarklist->nmark)
				buf->hmarklist->marks[a_href->hseq].invalid = 1;
			    a_href->hseq = -1;
			}
			a_href = NULL;
		    }
		    break;

		case HTML_LINK:
		    addLink(buf, tag);
		    break;

		case HTML_IMG_ALT:
		    if (parsedtag_get_value(tag, ATTR_SRC, &p)) {
#ifdef USE_IMAGE
			int w = -1, h = -1, iseq = 0, ismap = 0;
			int xoffset = 0, yoffset = 0, top = 0, bottom = 0;
			parsedtag_get_value(tag, ATTR_HSEQ, &iseq);
			parsedtag_get_value(tag, ATTR_WIDTH, &w);
			parsedtag_get_value(tag, ATTR_HEIGHT, &h);
			parsedtag_get_value(tag, ATTR_XOFFSET, &xoffset);
			parsedtag_get_value(tag, ATTR_YOFFSET, &yoffset);
			parsedtag_get_value(tag, ATTR_TOP_MARGIN, &top);
			parsedtag_get_value(tag, ATTR_BOTTOM_MARGIN, &bottom);
			if (parsedtag_exists(tag, ATTR_ISMAP))
			    ismap = 1;
			q = NULL;
			parsedtag_get_value(tag, ATTR_USEMAP, &q);
			if (iseq > 0) {
			    buf->imarklist = putHmarker(buf->imarklist,
							currentLn(buf), pos,
							iseq - 1);
			}
#endif
			s = NULL;
			parsedtag_get_value(tag, ATTR_TITLE, &s);
			p = url_quote_conv(remove_space(p),
					   buf->document_charset);
			a_img = registerImg(buf, p, s, currentLn(buf), pos);
#ifdef USE_IMAGE
			a_img->hseq = iseq;
			a_img->image = NULL;
			if (iseq > 0) {
			    ParsedURL u;
			    Image *image;

			    parseURL2(a_img->url, &u, base);
			    a_img->image = image = New(Image);
			    image->url = parsedURL2Str(&u)->ptr;
			    if (!uncompressed_file_type(u.file, &image->ext))
				image->ext = filename_extension(u.file, TRUE);
			    image->cache = NULL;
			    image->width =
				(w > MAX_IMAGE_SIZE) ? MAX_IMAGE_SIZE : w;
			    image->height =
				(h > MAX_IMAGE_SIZE) ? MAX_IMAGE_SIZE : h;
			    image->xoffset = xoffset;
			    image->yoffset = yoffset;
			    image->y = currentLn(buf) - top;
			    if (image->xoffset < 0 && pos == 0)
				image->xoffset = 0;
			    if (image->yoffset < 0 && image->y == 1)
				image->yoffset = 0;
			    image->rows = 1 + top + bottom;
			    image->map = q;
			    image->ismap = ismap;
			    image->touch = 0;
			    image->cache = getImage(image, base,
						    IMG_FLAG_SKIP);
			}
			else if (iseq < 0) {
			    BufferPoint *po = buf->imarklist->marks - iseq - 1;
			    Anchor *a = retrieveAnchor(buf->img,
						       po->line, po->pos);
			    if (a) {
				a_img->url = a->url;
				a_img->image = a->image;
			    }
			}
#endif
		    }
		    effect |= PE_IMAGE;
		    break;
		case HTML_N_IMG_ALT:
		    effect &= ~PE_IMAGE;
		    if (a_img) {
			a_img->end.line = currentLn(buf);
			a_img->end.pos = pos;
		    }
		    a_img = NULL;
		    break;
		case HTML_INPUT_ALT:
		    {
			FormList *form;
			int top = 0, bottom = 0;
			int textareanumber = -1;
#ifdef MENU_SELECT
			int selectnumber = -1;
#endif
			hseq = 0;
			form_id = -1;

			parsedtag_get_value(tag, ATTR_HSEQ, &hseq);
			parsedtag_get_value(tag, ATTR_FID, &form_id);
			parsedtag_get_value(tag, ATTR_TOP_MARGIN, &top);
			parsedtag_get_value(tag, ATTR_BOTTOM_MARGIN, &bottom);
			if (form_id < 0 || form_id > form_max || forms == NULL)
			    break;	/* outside of <form>..</form> */
			form = forms[form_id];
			if (hseq > 0) {
			    int hpos = pos;
			    if (*str == '[')
				hpos++;
			    buf->hmarklist =
				putHmarker(buf->hmarklist, currentLn(buf),
					   hpos, hseq - 1);
			}
			else if (hseq < 0) {
			    int h = -hseq - 1;
			    int hpos = pos;
			    if (*str == '[')
				hpos++;
			    if (buf->hmarklist &&
				h < buf->hmarklist->nmark &&
				buf->hmarklist->marks[h].invalid) {
				buf->hmarklist->marks[h].pos = hpos;
				buf->hmarklist->marks[h].line = currentLn(buf);
				buf->hmarklist->marks[h].invalid = 0;
				hseq = -hseq;
			    }
			}

			if (!form->target)
			    form->target = buf->baseTarget;
			if (a_textarea &&
			    parsedtag_get_value(tag, ATTR_TEXTAREANUMBER,
						&textareanumber)) {
			    if (textareanumber >= max_textarea) {
				max_textarea = 2 * textareanumber;
				textarea_str = New_Reuse(Str, textarea_str,
							 max_textarea);
				a_textarea = New_Reuse(Anchor *, a_textarea,
						       max_textarea);
			    }
			}
#ifdef MENU_SELECT
			if (a_select &&
			    parsedtag_get_value(tag, ATTR_SELECTNUMBER,
						&selectnumber)) {
			    if (selectnumber >= max_select) {
				max_select = 2 * selectnumber;
				select_option = New_Reuse(FormSelectOption,
							  select_option,
							  max_select);
				a_select = New_Reuse(Anchor *, a_select,
						     max_select);
			    }
			}
#endif
			a_form =
			    registerForm(buf, form, tag, currentLn(buf), pos);
			if (a_textarea && textareanumber >= 0)
			    a_textarea[textareanumber] = a_form;
#ifdef MENU_SELECT
			if (a_select && selectnumber >= 0)
			    a_select[selectnumber] = a_form;
#endif
			if (a_form) {
			    a_form->hseq = hseq - 1;
			    a_form->y = currentLn(buf) - top;
			    a_form->rows = 1 + top + bottom;
			    if (!parsedtag_exists(tag, ATTR_NO_EFFECT))
				effect |= PE_FORM;
			    break;
			}
		    }
		case HTML_N_INPUT_ALT:
		    effect &= ~PE_FORM;
		    if (a_form) {
			a_form->end.line = currentLn(buf);
			a_form->end.pos = pos;
			if (a_form->start.line == a_form->end.line &&
			    a_form->start.pos == a_form->end.pos)
			    a_form->hseq = -1;
		    }
		    a_form = NULL;
		    break;
		case HTML_MAP:
		    if (parsedtag_get_value(tag, ATTR_NAME, &p)) {
			MapList *m = New(MapList);
			m->name = Strnew_charp(p);
			m->area = newGeneralList();
			m->next = buf->maplist;
			buf->maplist = m;
		    }
		    break;
		case HTML_N_MAP:
		    /* nothing to do */
		    break;
		case HTML_AREA:
		    if (buf->maplist == NULL)	/* outside of <map>..</map> */
			break;
		    if (parsedtag_get_value(tag, ATTR_HREF, &p)) {
			MapArea *a;
			p = url_encode(remove_space(p), base,
				       buf->document_charset);
			t = NULL;
			parsedtag_get_value(tag, ATTR_TARGET, &t);
			q = "";
			parsedtag_get_value(tag, ATTR_ALT, &q);
			r = NULL;
			s = NULL;
#ifdef USE_IMAGE
			parsedtag_get_value(tag, ATTR_SHAPE, &r);
			parsedtag_get_value(tag, ATTR_COORDS, &s);
#endif
			a = newMapArea(p, t, q, r, s);
			pushValue(buf->maplist->area, (void *)a);
		    }
		    break;
		case HTML_FRAMESET:
		    frameset_sp++;
		    if (frameset_sp >= FRAMESTACK_SIZE)
			break;
		    frameset_s[frameset_sp] = newFrameSet(tag);
		    if (frameset_s[frameset_sp] == NULL)
			break;
		    if (frameset_sp == 0) {
			if (buf->frameset == NULL) {
			    buf->frameset = frameset_s[frameset_sp];
			}
			else
			    pushFrameTree(&(buf->frameQ),
					  frameset_s[frameset_sp], NULL);
		    }
		    else
			addFrameSetElement(frameset_s[frameset_sp - 1],
					   *(union frameset_element *)
					   &frameset_s[frameset_sp]);
		    break;
		case HTML_N_FRAMESET:
		    if (frameset_sp >= 0)
			frameset_sp--;
		    break;
		case HTML_FRAME:
		    if (frameset_sp >= 0 && frameset_sp < FRAMESTACK_SIZE) {
			union frameset_element element;

			element.body = newFrame(tag, buf);
			addFrameSetElement(frameset_s[frameset_sp], element);
		    }
		    break;
		case HTML_BASE:
		    if (parsedtag_get_value(tag, ATTR_HREF, &p)) {
			p = url_encode(remove_space(p), NULL,
				       buf->document_charset);
			if (!buf->baseURL)
			    buf->baseURL = New(ParsedURL);
			parseURL(p, buf->baseURL, NULL);
#if defined(USE_M17N) || defined(USE_IMAGE)
			base = buf->baseURL;
#endif
		    }
		    if (parsedtag_get_value(tag, ATTR_TARGET, &p))
			buf->baseTarget =
			    url_quote_conv(p, buf->document_charset);
		    break;
		case HTML_META:
		    p = q = NULL;
		    parsedtag_get_value(tag, ATTR_HTTP_EQUIV, &p);
		    parsedtag_get_value(tag, ATTR_CONTENT, &q);
		    if (p && q && !strcasecmp(p, "refresh") && MetaRefresh) {
			Str tmp = NULL;
			int refresh_interval = getMetaRefreshParam(q, &tmp);
#ifdef USE_ALARM
			if (tmp) {
			    p = url_encode(remove_space(tmp->ptr), base,
					   buf->document_charset);
			    buf->event = setAlarmEvent(buf->event,
						       refresh_interval,
						       AL_IMPLICIT_ONCE,
						       FUNCNAME_gorURL, p);
			}
			else if (refresh_interval > 0)
			    buf->event = setAlarmEvent(buf->event,
						       refresh_interval,
						       AL_IMPLICIT,
						       FUNCNAME_reload, NULL);
#else
			if (tmp && refresh_interval == 0) {
			    p = url_encode(remove_space(tmp->ptr), base,
					   buf->document_charset);
			    pushEvent(FUNCNAME_gorURL, p);
			}
#endif
		    }
		    break;
		case HTML_INTERNAL:
		    internal = HTML_INTERNAL;
		    break;
		case HTML_N_INTERNAL:
		    internal = HTML_N_INTERNAL;
		    break;
		case HTML_FORM_INT:
		    if (parsedtag_get_value(tag, ATTR_FID, &form_id))
			process_form_int(tag, form_id);
		    break;
		case HTML_TEXTAREA_INT:
		    if (parsedtag_get_value(tag, ATTR_TEXTAREANUMBER,
					    &n_textarea)
			&& n_textarea < max_textarea) {
			textarea_str[n_textarea] = Strnew();
		    }
		    else
			n_textarea = -1;
		    break;
		case HTML_N_TEXTAREA_INT:
		    if (n_textarea >= 0) {
			FormItemList *item =
			    (FormItemList *)a_textarea[n_textarea]->url;
			item->init_value = item->value =
			    textarea_str[n_textarea];
		    }
		    break;
#ifdef MENU_SELECT
		case HTML_SELECT_INT:
		    if (parsedtag_get_value(tag, ATTR_SELECTNUMBER, &n_select)
			&& n_select < max_select) {
			select_option[n_select].first = NULL;
			select_option[n_select].last = NULL;
		    }
		    else
			n_select = -1;
		    break;
		case HTML_N_SELECT_INT:
		    if (n_select >= 0) {
			FormItemList *item =
			    (FormItemList *)a_select[n_select]->url;
			item->select_option = select_option[n_select].first;
			chooseSelectOption(item, item->select_option);
			item->init_selected = item->selected;
			item->init_value = item->value;
			item->init_label = item->label;
		    }
		    break;
		case HTML_OPTION_INT:
		    if (n_select >= 0) {
			int selected;
			q = "";
			parsedtag_get_value(tag, ATTR_LABEL, &q);
			p = q;
			parsedtag_get_value(tag, ATTR_VALUE, &p);
			selected = parsedtag_exists(tag, ATTR_SELECTED);
			addSelectOption(&select_option[n_select],
					Strnew_charp(p), Strnew_charp(q),
					selected);
		    }
		    break;
#endif
		case HTML_TITLE_ALT:
		    if (parsedtag_get_value(tag, ATTR_TITLE, &p))
			buf->buffername = html_unquote(p);
		    break;
		case HTML_SYMBOL:
		    effect |= PC_SYMBOL;
		    if (parsedtag_get_value(tag, ATTR_TYPE, &p))
			symbol = (char)atoi(p);
		    break;
		case HTML_N_SYMBOL:
		    effect &= ~PC_SYMBOL;
		    break;
		}
#ifdef	ID_EXT
		id = NULL;
		if (parsedtag_get_value(tag, ATTR_ID, &id)) {
		    id = url_quote_conv(id, name_charset);
		    registerName(buf, id, currentLn(buf), pos);
		}
		if (renderFrameSet &&
		    parsedtag_get_value(tag, ATTR_FRAMENAME, &p)) {
		    p = url_quote_conv(p, buf->document_charset);
		    if (!idFrame || strcmp(idFrame->body->name, p)) {
			idFrame = search_frame(renderFrameSet, p);
			if (idFrame && idFrame->body->attr != F_BODY)
			    idFrame = NULL;
		    }
		}
		if (id && idFrame)
		    idFrame->body->nameList =
			putAnchor(idFrame->body->nameList, id, NULL,
				  (Anchor **)NULL, NULL, NULL, '\0',
				  currentLn(buf), pos);
#endif				/* ID_EXT */
	    }
	}
	/* end of processing for one line */
	if (!internal)
	    addnewline(buf, outc, outp, NULL, pos, -1, nlines);
	if (internal == HTML_N_INTERNAL)
	    internal = 0;
	if (str != endp) {
	    line = Strsubstr(line, str - line->ptr, endp - str);
	    goto proc_again;
	}
    }
#ifdef DEBUG
    if (w3m_debug)
	fclose(debug);
#endif
    for (form_id = 1; form_id <= form_max; form_id++)
	forms[form_id]->next = forms[form_id - 1];
    buf->formlist = (form_max >= 0) ? forms[form_max] : NULL;
    if (n_textarea)
	addMultirowsForm(buf, buf->formitem);
#ifdef USE_IMAGE
    addMultirowsImg(buf, buf->img);
#endif
}