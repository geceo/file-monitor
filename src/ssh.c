#include <stdio.h>
#include <stdlib.h>
#include <libssh/libssh.h>
#include <libssh/sftp.h>

#include "logs.h"
#include "fmond_plugin.h"


typedef struct fmplug_ssh_t {
	ssh_session sess;
	sftp_session sftp;
} fmplug_ctx_t;


void cleanup(fmplug_ssh_t *ctx)
{
	sftp_free(ctx->sftp);
	ssh_disconnect(ctx->sess);
	ssh_free(ctx->sess);

}

int directory_update (char *path, uint32_t mask)
{
	return 0;
}

int file_update (char *path, uint32_t mask)
{
	return 0;
}

fmond_plugin_t *init ()
{
	char *host, *user, *password;
	int port;

	fmplug_ssh_t *ctx;
	fmond_plugin_t *plugin_ctx;

	host = "localhost";
	user = "fmond";
	password = "fmond";

	plugin_ctx = malloc(sizeof(fmond_plugin_t));
	ctx = malloc(sizeof(fmplug_ssh_t));

	if (!(ctx->sess = ssh_new()))
		goto err;

	ssh_options_set(ctx->sess, SSH_OPTIONS_HOST, host);
	ssh_options_set(ctx->sess, SSH_OPTIONS_USER, user);

	if (ssh_connect(ctx->sess) != SSH_OK)
		goto conn_err;
	
	if (ssh_userauth_password(ctx->sess, NULL, password) != SSH_AUTH_SUCCESS)
		goto conn_err;

	if (!(ctx->sftp = sftp_new(ctx->sess)))
		goto sftp_err;	
	
	return (void *)ctx;

sftp_err:
	sftp_free(ctx->sftp);
conn_err:
	ssh_disconnect(ctx->sess);
err:
	fprintf(stderr,"ssh error:  %s\n",ssh_get_error(ctx->sess));
	ssh_free(ctx->sess);
	return NULL;
}



#ifdef UNIT_TEST

#endif
