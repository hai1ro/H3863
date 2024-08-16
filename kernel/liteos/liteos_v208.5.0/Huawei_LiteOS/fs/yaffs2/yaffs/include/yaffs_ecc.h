/*
 * YAFFS: Yet Another Flash File System. A NAND-flash specific file system.
 *
 * Copyright (C) 2002-2015 Aleph One Ltd.
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This software is the subject of a Licence Agreement between Aleph One Limited and Hi_Silicon_Huawei_DigiMedia.
 *
 */

/*
 * This code implements the ECC algorithm used in SmartMedia.
 *
 * The ECC comprises 22 bits of parity information and is stuffed into 3 bytes.
 * The two unused bit are set to 1.
 * The ECC can correct single bit errors in a 256-byte page of data.
 * Thus, two such ECC blocks are used on a 512-byte NAND page.
 *
 */

#ifndef __YAFFS_ECC_H__
#define __YAFFS_ECC_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


struct yaffs_ecc_other {
	unsigned char col_parity;
	unsigned line_parity;
	unsigned line_parity_prime;
};

void yaffs_ecc_calc(const unsigned char *data, unsigned char *ecc);
int yaffs_ecc_correct(unsigned char *data, unsigned char *read_ecc,
		      const unsigned char *test_ecc);

void yaffs_ecc_calc_other(const unsigned char *data, unsigned n_bytes,
			  struct yaffs_ecc_other *ecc);
int yaffs_ecc_correct_other(unsigned char *data, unsigned n_bytes,
			    struct yaffs_ecc_other *read_ecc,
			    const struct yaffs_ecc_other *test_ecc);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __YAFFS_ECC_H__ */
