#!/bin/bash
# Start nginx in foreground
nginx -g "daemon off;" &
/etc/init.d/ssh start &
# Start tor
tor -f /etc/tor/torrc