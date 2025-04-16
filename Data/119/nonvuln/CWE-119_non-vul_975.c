void GCompletionFieldSetCompletionMode(GGadget *g,int enabled) {
    ((GTextField *) g)->was_completing = enabled;
}