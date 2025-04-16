void SVGInlineTextBox::dirtyLineBoxes()
{
    InlineTextBox::dirtyLineBoxes();

    // Clear the now stale text fragments
    clearTextFragments();
}