/* gtkui.c */

#include "gtkui.h"
#include "display.h"

#ifdef HAVE_GTK

#include "../ifacegen/ifacegen.h"
#include "../cpc/fdc.h"
#include "../cpc/arnold.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <gtk/gtk.h>

GtkWidget *btn_diska, *btn_diskb, *btn_reset, *btn_quit, *btn_lock,
	*btn_double;

gint delete_event( GtkWidget *widget, GdkEvent *event, gpointer data ) {
	return( FALSE );	/* Emit a destroy event */
}

void destroy( GtkWidget *widget, gpointer data ) {
	gtk_main_quit();
}

void choosen_disk( GtkWidget *w, GtkFileSelection *fs, int drive ) {

	static char filename[ PATH_MAX ];

    strncpy( filename,
		gtk_file_selection_get_filename( GTK_FILE_SELECTION(fs) ),
		PATH_MAX );
	gtk_widget_destroy( GTK_WIDGET(fs) );
    g_print( "%s\n", filename );
	/*if (FDD_IsDiskPresent(drive)) {
		GenericInterface_RemoveDiskImage(drive);
	}*/
	if (!GenericInterface_InsertDiskImage( drive, filename )) {
		printf("Failed to open disk image %s.\r\n", filename);
	} 

}

void choosen_diska( GtkWidget *w, GtkFileSelection *fs ) {

		choosen_disk( w, fs, 0 );
}

void choosen_diskb( GtkWidget *w, GtkFileSelection *fs ) {

		choosen_disk( w, fs, 1 );

}

void choose_disk( GtkWidget *widget, gpointer data ) {

	GtkWidget *filew;
	char *title;
	GtkSignalFunc function;

	if ( data == btn_diska ) {
			title = "Drive A";
			function = (GtkSignalFunc) choosen_diska;
	} else if ( data == btn_diskb ) {
			title = "Drive B";
			function = (GtkSignalFunc) choosen_diskb;
	} else {
		fprintf( stderr, "Unexcpected error!" );
		exit( -1 );
	}

	filew = gtk_file_selection_new( title );
	
	gtk_signal_connect( GTK_OBJECT(GTK_FILE_SELECTION(filew)->ok_button),
		"clicked", function, filew );

	gtk_signal_connect_object( GTK_OBJECT(GTK_FILE_SELECTION(
		filew)->cancel_button),
		"clicked", (GtkSignalFunc) gtk_widget_destroy,
		GTK_OBJECT (filew) );

	gtk_widget_show(filew);

}

//void choose_diskb( GtkWidget *widget, gpointer data ) {
//}

void reset( GtkWidget *widget, gpointer data ) {
	GenericInterface_DoReset();
}

void quit( GtkWidget *widget, gpointer data ) {
	gtk_main_quit();
}

void throttle( GtkWidget *widget, gpointer data ) {
#ifdef HAVE_SDL
	sdl_LockSpeed = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(data));
#endif
}

void doubledisp( GtkWidget *widget, gpointer data ) {
#ifdef HAVE_SDL
	//sdl_LockSpeed = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(data));
	sdl_SetDoubled(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(data)));
#endif
}

GtkWidget *make_button( char *label, void *click ) {

	GtkWidget *button = gtk_button_new_with_label( label );
	gtk_signal_connect( GTK_OBJECT(button), "clicked",
		GTK_SIGNAL_FUNC(click), button);
	gtk_widget_show( button );
	return button;

}

GtkWidget *make_button_in_box( char *label, void *click, GtkWidget *box ) {

	GtkWidget *button = make_button( label, click );
	/*                                       expand,  fill, padding */
	gtk_box_pack_start( GTK_BOX(box), button, FALSE, FALSE, 0 );
	return button;

}

#if 0
GtkWidget *make_toggle_button( char *label, void *toggle ) {

	GtkWidget *toggle_button = gtk_toggle_button_new_with_label( label );
	gtk_signal_connect( GTK_OBJECT(toggle_button), "toggled",
		GTK_SIGNAL_FUNC(toggle), toggle_button);
	gtk_widget_show( toggle_button );
	return toggle_button;

}

GtkWidget *make_toggle_button_in_box( char *label, void *toggle,
	GtkWidget *box ) {

	GtkWidget *toggle_button = make_toggle_button( label, toggle );
	/*                                       expand,  fill, padding */
	gtk_box_pack_start( GTK_BOX(box), toggle_button, FALSE, FALSE, 0 );
	return toggle_button;

}
#endif

GtkWidget *make_check_button( char *label, void *toggle ) {

	GtkWidget *check_button = gtk_check_button_new_with_label( label );
	gtk_signal_connect( GTK_OBJECT(check_button), "toggled",
		GTK_SIGNAL_FUNC(toggle), check_button);
	gtk_widget_show( check_button );
	return check_button;

}

GtkWidget *make_check_button_in_box( char *label, void *toggle,
	GtkWidget *box ) {

	GtkWidget *check_button = make_check_button( label, toggle );
	/*                                       expand,  fill, padding */
	gtk_box_pack_start( GTK_BOX(box), check_button, FALSE, FALSE, 0 );
	return check_button;

}

void gtkui_init( int argc, char **argv ) {

	GtkWidget *window;
	GtkWidget *box, *box_disk, *box_control, *box_settings;
	GtkWidget *frm_disk, *frm_control, *frm_settings;

	/* Init GUI */
	gtk_init( &argc, &argv );

	/* Init Window */
	window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
	gtk_signal_connect( GTK_OBJECT (window), "delete_event",
		GTK_SIGNAL_FUNC(delete_event), NULL );
	gtk_signal_connect( GTK_OBJECT (window), "destroy",
		GTK_SIGNAL_FUNC(destroy), NULL );
	gtk_container_set_border_width( GTK_CONTAINER (window), 10 );

	/* Init Frames and Boxes */
	/*            homogeneous, spacing */
	box = gtk_vbox_new( FALSE, 0 );
	box_disk = gtk_vbox_new( FALSE, 0 );
	box_control = gtk_vbox_new( FALSE, 0 );
	box_settings = gtk_vbox_new( FALSE, 0 );
	frm_disk = gtk_frame_new( "Disk" );
	frm_control = gtk_frame_new( "Control" );
	frm_settings = gtk_frame_new( "Settings" );

	/* Init buttons */
	btn_diska = make_button_in_box( "Drive A", choose_disk, box_disk );
	btn_diskb = make_button_in_box( "Drive B", choose_disk, box_disk );
	btn_reset = make_button_in_box( "Reset", reset, box_control );
	btn_quit = make_button_in_box( "Quit", quit, box_control );
	btn_lock = make_check_button_in_box( "Lock Speed", throttle, box_settings );
	btn_double = make_check_button_in_box( "Double Display", doubledisp,
		box_settings );

	gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON (btn_lock), TRUE );

	gtk_container_add( GTK_CONTAINER(frm_disk), box_disk );
	gtk_container_set_border_width( GTK_CONTAINER (box_disk), 5 );
	gtk_widget_show( box_disk );
	gtk_container_add( GTK_CONTAINER(frm_control), box_control );
	gtk_container_set_border_width( GTK_CONTAINER (box_control), 5 );
	gtk_widget_show( box_control );
	gtk_container_add( GTK_CONTAINER(frm_settings), box_settings );
	gtk_container_set_border_width( GTK_CONTAINER (box_settings), 5 );
	gtk_widget_show( box_settings );
	gtk_container_add( GTK_CONTAINER(box), frm_disk );
	gtk_widget_show( frm_disk );
	gtk_container_add( GTK_CONTAINER(box), frm_control );
	gtk_widget_show( frm_control );
	gtk_container_add( GTK_CONTAINER(box), frm_settings );
	gtk_widget_show( frm_settings );
	gtk_container_add( GTK_CONTAINER(window), box );
	gtk_widget_show( box );

	/* Show GUI */
	gtk_widget_show( window );

	/* Start GTK+ main loop in separate thread.
	 * Hopefully this is a good idea... */
#if 0
	if ( fork() == 0 ) {
		gtk_main();			/* GTK+ main loop */
		kill( 0, SIGTERM );	/* Kill process when exiting GTK+ main loop */
		exit( 0 );
	}
#endif

}

int idlerun( gpointer data ) {
		//fprintf(stderr,".");
	    CPCEmulation_Run();
		return TRUE;
}

void gtkui_run( void ) {
		gtk_idle_add( idlerun, NULL );
		//gtk_timeout_add( 100, idlerun, NULL );
		gtk_main();			/* GTK+ main loop */
		printf("Finished gtk_main()\n");
}

#endif	/* HAVE_GTK */

