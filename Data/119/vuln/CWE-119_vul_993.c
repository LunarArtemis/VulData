static int vfio_msi_enable(struct vfio_pci_device *vdev, int nvec, bool msix)
{
	struct pci_dev *pdev = vdev->pdev;
	unsigned int flag = msix ? PCI_IRQ_MSIX : PCI_IRQ_MSI;
	int ret;

	if (!is_irq_none(vdev))
		return -EINVAL;

	vdev->ctx = kzalloc(nvec * sizeof(struct vfio_pci_irq_ctx), GFP_KERNEL);
	if (!vdev->ctx)
		return -ENOMEM;

	/* return the number of supported vectors if we can't get all: */
	ret = pci_alloc_irq_vectors(pdev, 1, nvec, flag);
	if (ret < nvec) {
		if (ret > 0)
			pci_free_irq_vectors(pdev);
		kfree(vdev->ctx);
		return ret;
	}

	vdev->num_ctx = nvec;
	vdev->irq_type = msix ? VFIO_PCI_MSIX_IRQ_INDEX :
				VFIO_PCI_MSI_IRQ_INDEX;

	if (!msix) {
		/*
		 * Compute the virtual hardware field for max msi vectors -
		 * it is the log base 2 of the number of vectors.
		 */
		vdev->msi_qmax = fls(nvec * 2 - 1) - 1;
	}

	return 0;
}