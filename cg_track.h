/* prepare for mouse-trackball operation     */
/* This module should be called by main func */
void ControlMouse_track(void);

/* translation of view position                 */
/* This module should be called by display func */
void ViewFromMouse_track(void);


/* mouse callback function */
void Mouse_track(int, int, int, int);
void Motion_track(int, int);

