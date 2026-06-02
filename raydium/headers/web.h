#ifndef _WEB_H
#define _WEB_H

#include "../web.h"

/*=
HTTP Web server and tools
4200
**/

// Introduction
/**
Raydium applications can embed a small HTTP server. This server is used to be
an entry point to application data. Only simple requests (GET) are supported,
with a limited set of file types. Right now, this server is able to send
static and dynamic data, and dynamic scripted page support is to come, using
Raydium's PHP parser.

The server is a modified version of IBM's nweb server, from
Nigel Griffiths (nag AT uk DOT ibm DOT com).

Raydium also provide a very small HTTP client, useful to get data (tracks,
maps, sprays, ...) from the game server.

To set up a web server in your application, you'll need to do a few things.
First, make sure that HTTP support is enabled (see ##raydium_web_init()##) and
that your HTTP server is started (##raydium_web_start()##).

The HTTP server will use TCP port ##RAYDIUM_NETWORK_PORT## (29104), therefore
a typical URL to reach the server is something like [[http://127.0.0.1:29104]]
where you'll find your index page. If the HTTP server (and your application)
is started (see above), you can point your browser to this URL right now.

The default index page can be changed thru ##raydium_web_body_default##
variable :
%%
char *index_text="\
<br/><center>This is a <b>test</b> server. You can know more about Raydium by\
<a href=\"http://maniadrive.raydium.org/\">by clicking here</a></center>";

...

int main(int argc, char **argv)
{
...
raydium_web_start("test server");
raydium_web_body_default=index_text;
}
%%

You can also change default header and footer the same way, using
##raydium_web_header## and ##raydium_web_footer## variables.

Then you may need to register "file" extensions (static data or dynamic
handler) using ##raydium_web_extension_add()## function.

Please note that Raydium's not Apache ! You're supposed to serve light
pages, with almost no processing. The server is not even threaded, so you
may hit hardly framerate not following these recommendations, or if too
many HTTP requests are sent concurrently to the server.
**/

__rayapi void raydium_web_answer(char *message, int fd);
/**
Internal use. Default HTTP handler (HTML message).
**/

__rayapi void raydium_web_request(int fd);
/**
Internal use. Will decode HTTP client request.
**/

__rayapi void raydium_web_start(char *title);
/**
Will start the Raydium embedded HTTP server. The TCP port 29104 must be free.
The ##title## will be used in HTTP headers and in the default HTML header.
**/

__rayapi void raydium_web_callback(void);
/**
Internal use. Will accept any pending connection.
**/

__rayapi void raydium_web_init(void);
/**
You should not have to call this function by yourself, unless your application
is a game server. (see ##raydium_network_only_init()## for more informations
about network only applications)
**/

__rayapi void raydium_web_extension_add(char *ext, char *mime, void *handler);
/**
This function will register a new file extension to the web server.

Note that Raydium HTTP server have no idea of URL parameters and will consider
that everything after the root slash in the URL is the requested "filename".

So the ##ext## could be of any size and will only try to match the end
of URLs ("tga" or ".tga" for instance).
As an example, an URL like http://myhost/toto.php?f=a.tga **will match**
the extension ".tga" (the filename will be ##toto.php?f=a.tga##, here).
But this one will not: http://myhost/toto.php?f=a.tga&r=12

This behavior may change soon, so please contact us before doing anything
complex with extension.


You can also set a ##handler## for your extension. A sample handler may
look like this:
%%(c)
signed char http_req(char *req, char *response, int max_size)
{
if(!strcmp("a.dyn",req))
    {
    sprintf(response,"This a sample for <b>a.dyn</b>");
    return 1;
    }
if(!strcmp("b.dyn",req))
    {
    sprintf(response,"This a sample for <b>b.dyn</b>");
    return 1;
    }

return 0;
}
%%

In this handler, ##req## is the requested "filename" (see above about
extensions) and ##response## is a pre-allocated buffer (of ##max_size## bytes,
usually 8 kB) where you must write your response data/text. The hanlder
must return ##1## if everything is correct, and ##0## to deny the request.

If you set ##handler## to NULL, the HTTP server will sent the requested file
directly to the browser.


Then you can define a ##mime## type, particularly if you set ##handler## to
##NULL##, so the file is sent with a correct MIME type ("raw/unknown",
"text/plain", ...).
When using a handler, you should probably set ##mime## to ##NULL##, since it
will then use the default HTML Raydium handler. But you can even set ##mime##
type when using your handlers, allowing you to create fully custom responses.
**/

__rayapi signed char raydium_web_client_get(char *filename);
/**
This is a small HTTP 1.0 client, used to download data from a Raydium server.
For any other use, you should use PHP (See PHP and RayPHP chapters).

To use this function you must be connected to a game server (see
##raydium_network_client_connect_to()## for more informations), therefore
##filename## should be only a path or a filename, not an URL.

Warning: no proxy support is provided here.

This function is very useful for downloading data from the game server
you're currently connected to, like tracks, maps, skins, ...
This client is able to detect Raydium HTTP server messages, so it will not
download HTML content instead of data when a Raydium server returns an error.

It will not overwrite the local file if the downloaded one is the same.

No upload support is available yet.
**/

#endif
