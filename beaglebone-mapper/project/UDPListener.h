// UDPListener.h
// Module to spawn a separate thread to listen for UDP instructions and
// and process commands
#ifndef _UDP_LISTENER_H_
#define _UDP_LISTENER_H_
// Start up UDP thread
void UDP_ListenerLaunchThread(void);
// Listening thread function
void UDP_close(void);
#endif

