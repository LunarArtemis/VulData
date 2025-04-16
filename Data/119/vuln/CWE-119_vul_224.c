static int host_start(struct ci13xxx *ci)
{
	struct usb_hcd *hcd;
	struct ehci_hcd *ehci;
	int ret;

	if (usb_disabled())
		return -ENODEV;

	hcd = usb_create_hcd(&ci_ehci_hc_driver, ci->dev, dev_name(ci->dev));
	if (!hcd)
		return -ENOMEM;

	dev_set_drvdata(ci->dev, ci);
	hcd->rsrc_start = ci->hw_bank.phys;
	hcd->rsrc_len = ci->hw_bank.size;
	hcd->regs = ci->hw_bank.abs;
	hcd->has_tt = 1;

	hcd->power_budget = ci->platdata->power_budget;
	hcd->phy = ci->transceiver;

	ehci = hcd_to_ehci(hcd);
	ehci->caps = ci->hw_bank.cap;
	ehci->has_hostpc = ci->hw_bank.lpm;

	ret = usb_add_hcd(hcd, 0, 0);
	if (ret)
		usb_put_hcd(hcd);
	else
		ci->hcd = hcd;

	return ret;
}