/* 
 * cancel.c - cancel a slurm job
 * see slurm.h for documentation on external functions and data structures
 *
 * author: moe jette, jette@llnl.gov
 */

#define DEBUG_SYSTEM 1

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <src/api/slurm.h>
#include <src/common/slurm.h>

#if DEBUG_MODULE
/* main is used here for testing purposes only */
int 
main (int argc, char *argv[]) 
{
	int error_code = 0, i;

	if (argc < 2) {
		printf ("Usage: %s job_id\n", argv[0]);
		exit (1);
	}

	for (i=1; i<argc; i++) {
		error_code = slurm_cancel ((uint32_t) atoi(argv[i]));
		if (error_code != 0)
			printf ("slurm_cancel error %d for job %s\n", 
				error_code, argv[i]);
	}

	exit (error_code);
}
#endif

slurm_cancel_job ( uint32_t job_id )
{
        int msg_size ;
        int rc ;
        slurm_fd sockfd ;
        slurm_msg_t request_msg ;
        slurm_msg_t response_msg ;
	job_id_msg_t job_id_msg ;
        return_code_msg_t * rc_msg ;

        /* init message connection for message communication with controller */
        if ( ( sockfd = slurm_open_controller_conn ( SLURM_PORT ) ) == SLURM_SOCKET_ERROR )
                return SLURM_SOCKET_ERROR ;

        /* send request message */
	job_id_msg . job_id = job_id ;
        request_msg . msg_type = REQUEST_JOB_CANCEL ;
        request_msg . data = &job_id_msg ;
        if ( ( rc = slurm_send_controller_msg ( sockfd , & request_msg ) ) == SLURM_SOCKET_ERROR )
                return SLURM_SOCKET_ERROR ;

        /* receive message */
        if ( ( msg_size = slurm_receive_msg ( sockfd , & response_msg ) ) == SLURM_SOCKET_ERROR )
                return SLURM_SOCKET_ERROR ;
        /* shutdown message connection */
        if ( ( rc = slurm_shutdown_msg_conn ( sockfd ) ) == SLURM_SOCKET_ERROR )
                return SLURM_SOCKET_ERROR ;

        switch ( response_msg . msg_type )
        {
                case RESPONSE_SLURM_RC:
                        rc_msg = ( return_code_msg_t * ) response_msg . data ;
			return rc_msg -> return_code ;
                        break ;
                default:
                        return SLURM_UNEXPECTED_MSG_ERROR ;
                        break ;
        }

        return SLURM_SUCCESS ;
}
