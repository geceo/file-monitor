#include <stdio.h>
#include <libssh/libssh.h>
#include "logs.h"


struct fmplug_ssh_t {
	ssh_session *sess;
	sftp_session *sftp;
};

#define ssh_ctx fmplug_ssh_t

void cleanup(ssh_ctx *ctx)
{

}

static void *init (char *host, char *user, char *password, int port)
{
	struct ssh_ctx *ctx;

	ctx = malloc(sizeof(struct ssh_ctx));

	if (!(ctx->sess = ssh_new()))
		goto err;

	ssh_options_set(ctx->sess, SSH_OPTIONS_HOST, av[1]);
	ssh_options_set(ctx->sess, SSH_OPTIONS_USER, av[2]);

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
	ssh_disconnect(ctx->);
err:
	if (sess)
			fprintf(stderr,"error connecting %s: %s\n",ssh_get_error(sess));

	ssh_free(sess);
	return NULL;
}



#ifdef UNIT_TEST

#endif
