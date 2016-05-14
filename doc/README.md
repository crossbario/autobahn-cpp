Autobahn|CPP Documentation
==================

> This documentation is quite outdated (and also woefully incomplete). When in doubt ask on the [mailing list](https://groups.google.com/forum/#!forum/autobahnws) or read the code!

> Contributions to this documentation are highly welcome!


Autobahn|CPP is an open-source networking library for C++ created by the [Autobahn project ](http://autobahn.ws/) that implements the [Web Application Messaging Protocol (WAMP)]( http://wamp.ws/) for creating C++ components communicating with a WAMP router via Unix pipes.


Web Application Messaging Protocol (WAMP)
-----------------------------------------

WAMP is ideal for distributed, multi-client and server applications, such as multi-user database-drive business applications, sensor networks (IoT), instant messaging or MMOGs (massively multi-player online games) .

WAMP enables application architectures with application code distributed freely across processes and devices according to functional aspects. Since WAMP implementations exist for multiple languages, WAMP applications can be polyglott. Application components can be implemented in a language and run on a device which best fit the particular use case.

To enable this, WAMP implements both the Remote Procedure Call (RPC) and  and the Publish and Subscribe (PubSub) messaging pattern. WAMP is open source, as is Autobahn|CPP (Apache 2.0 license).
