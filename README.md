# Protocol stack for OSMORA ONET infrastructure

## What is O.1P?

O.1P is a minimal, fast and lightweight LAN protocol which can be used for
various applications such as file sharing, shared logs, printer servers, etc.

It is mainly intended for use with OSMORA infrastructure, however anyone is
welcome to deploy their own instance for personal usage.

## Make sure to assign an IP to your ethernet interface!:

```sh
ip addr add 192.168.5.11/24 dev eth0
```

## Build instructions

```sh
make
doas make install
```

An example program is in ``examples/`` and can be compiled with
the following commands (assuming an install was made):

```sh
gcc -lonet examples/hello.c
```
