/*
 * Copyright 2014 Tushar Gohad, Kevin M Greenan, Eric Lambert
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.  THIS SOFTWARE IS PROVIDED BY
 * THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * liberasurecode postprocessing helpers implementation
 *
 * vi: set noai tw=79 ts=4 sw=4:
 */

#include "erasurecode_backend.h"
#include "erasurecode_helpers.h"
#include "erasurecode_stdinc.h"


//whcho added
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "galois.h"
#include <galois.h>





void add_fragment_metadata(ec_backend_t be, char *fragment,
        int idx, uint64_t orig_data_size, int blocksize,
        ec_checksum_type_t ct, int add_chksum)
{
    //TODO EDL we are ignoring the return codes here, fix that
    set_libec_version(fragment);
    set_fragment_idx(fragment, idx);
    set_orig_data_size(fragment, orig_data_size);
    set_fragment_payload_size(fragment, blocksize);
    set_backend_id(fragment, be->common.id);
    set_backend_version(fragment, be->common.ec_backend_version);
    set_fragment_backend_metadata_size(fragment, be->common.backend_metadata_size);

    if (add_chksum) {
        set_checksum(ct, fragment, blocksize);
    }
}

int finalize_fragments_after_encode(ec_backend_t instance,
        int k, int m, int blocksize, uint64_t orig_data_size,
        char **encoded_data, char **encoded_parity)
{
    int i, set_chksum = 1;
    ec_checksum_type_t ct = instance->args.uargs.ct;

//whcho added    
char **ori_data; // contain the original data from get_fragment_ptr_from_data(encoded_data[i] or encoded_parity[i]);
char **p_coding; // to be stored as addtional 6 parities after GF arithmetic
int fragmentsize; // get its fragmentsize
int j ; // for loop 




//for testing whcho 
char p_fname[100];
FILE *p_fp;







    /* finalize data fragments */
    for (i = 0; i < k; i++) {
        char *fragment = get_fragment_ptr_from_data(encoded_data[i]);
        add_fragment_metadata(instance, fragment, i, orig_data_size,
                blocksize, ct, set_chksum);
        encoded_data[i] = fragment;
    }

    /* finalize parity fragments */
    for (i = 0; i < m; i++) {
        char *fragment = get_fragment_ptr_from_data(encoded_parity[i]);
        add_fragment_metadata(instance, fragment, i + k, orig_data_size,
                blocksize, ct, set_chksum);
        encoded_parity[i] = fragment;
    }

//whcho added 
//memcpy(ori_data[0], encoded_data[0], blocksize);
//memcpy(ori_data[1], encoded_data[1], blocksize);
//memcpy(ori_data[2], encoded_data[2], blocksize);


fragmentsize = get_fragment_size(encoded_data[0]); // get a encoded data / parity size !

// for testing whcho
ori_data = (char **)malloc(sizeof(char*)*3); // initialize!! 3 packets at each node will be mixed
for(i=0 ;i<3;i++){
	//ori_data[i]=(char*)malloc(sizeof(char)* (blocksize+80));
	ori_data[i]=(char*)malloc(sizeof(char)* (fragmentsize));
	//ori_data[i]=(char*)malloc(sizeof(char)* (blocksize));
	if( ori_data[i] == NULL ){ perror("malloc"); exit(1);}

}

//char *test_fragment = get_fragment_ptr_from_data(encoded_data[0]); // should be copied into char pointer in order to copy its data !
//test_fragment = encoded_data[0]; 
//memcpy(ori_data[0], test_fragment , fragmentsize );

sprintf(p_fname, "/home/whcho/pyeclib/test/test_fragments/ori_data_0.mp4");
p_fp = fopen(p_fname, "wb");
fwrite(ori_data[0], sizeof(char), fragmentsize, p_fp);
fclose(p_fp);


ori_data[0]=encoded_data[0];
ori_data[1]=encoded_data[1];
ori_data[2]=encoded_data[2];


// each node have to create 1 parity using its local 3 data_fragments
p_coding = (char **)malloc(sizeof(char*)*10); 
for(i=0 ;i<10;i++){
	p_coding[i]=(char*)malloc(sizeof(char)* (fragmentsize));
	if( p_coding[i] == NULL ){ perror("malloc"); exit(1);}

}



for(j=0 ; j<k ; j++){ //data fragment

	//ori_data[(3*j)]=encoded_data[(3*j)];
	//ori_data[(3*j)+1]=encoded_data[(3*j)+1];
	//ori_data[(3*j)+2]=encoded_data[(3*j)+2];

	galois_w08_region_multiply(encoded_data[(3*j)], 1 , fragmentsize, p_coding[j], 0);
	galois_w08_region_multiply(encoded_data[(3*j)+1], 1 , fragmentsize, p_coding[j], 1);
	galois_w08_region_multiply(encoded_data[(3*j)+2], 1 , fragmentsize, p_coding[j], 1);


}

for(j=k ; j < m+k ; j++){ // parity fragment

	//ori_data[(3*j)]=encoded_data[(3*j)];
	//ori_data[(3*j)+1]=encoded_data[(3*j)+1];
	//ori_data[(3*j)+2]=encoded_data[(3*j)+2];

	galois_w08_region_multiply(encoded_data[(3*j)], 1 , fragmentsize, p_coding[j], 0);
	galois_w08_region_multiply(encoded_data[(3*j)+1], 1 , fragmentsize, p_coding[j], 1);
	galois_w08_region_multiply(encoded_data[(3*j)+2], 1 , fragmentsize, p_coding[j], 1);


}

// making 6 parities content
for(i=0 ; i<6 ; i++){

	char *fragment = get_fragment_ptr_from_data(encoded_parity[6+i]);
        fragment = ori_data[0]; // because of size , cannot copy correctly ! -> so need to modify ori_data itself! 
	//fragment = encoded_data[0+i];
	encoded_parity[6+i] = fragment;

}




/*char **temp;
        *temp = get_fragment_ptr_from_data(encoded_parity[6]);
	*temp = encoded_data[0];
        encoded_parity[6] = *temp;
*/




    return 0;
}
