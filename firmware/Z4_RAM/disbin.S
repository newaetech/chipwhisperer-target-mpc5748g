
simpleserial_Z4_1-CW308_MPC5748G.bin:     file format binary


Disassembly of section .data:

00000000 <.data>:
   0:	70 e0 60 00 	e_li    r7,-262144
   4:	48 15       	se_li   r5,1
   6:	50 c7 02 fc 	e_lwz   r6,764(r7)
   a:	74 a6 c9 8c 	e_rlwimi r6,r5,25,6,6
   e:	54 c7 02 fc 	e_stw   r6,764(r7)
  12:	30 c7 13 2f 	e_lbz   r6,4911(r7)
  16:	65 f6       	se_bseti r6,31
  18:	34 c7 13 2f 	e_stb   r6,4911(r7)
  1c:	00 04       	se_blr
	...
  fe:	00 00       	se_illegal
 100:	78 00 02 b4 	e_b     0x3b4
	...
 200:	70 68 e0 03 	e_lis   r3,16387
 204:	70 e8 e0 03 	e_lis   r7,16387
 208:	1c 63 04 44 	e_add16i r3,r3,1092
 20c:	1c e7 04 44 	e_add16i r7,r7,1092
 210:	20 27       	se_addi r7,3
 212:	06 37       	se_sub  r7,r3
 214:	22 57       	se_cmpli r7,6
 216:	7a 01 00 16 	e_ble   0x22c
 21a:	70 e0 e0 00 	e_lis   r7,0
 21e:	1c e7 00 00 	e_add16i r7,r7,0
 222:	2a 07       	se_cmpi r7,0
 224:	7a 12 00 08 	e_beq   0x22c
 228:	00 b7       	se_mtctr r7
 22a:	00 06       	se_bctr
 22c:	00 04       	se_blr
 22e:	44 00       	se_nop
 230:	70 68 e0 03 	e_lis   r3,16387
 234:	70 88 e0 03 	e_lis   r4,16387
 238:	1c 63 04 44 	e_add16i r3,r3,1092
 23c:	1c 84 04 44 	e_add16i r4,r4,1092
 240:	06 34       	se_sub  r4,r3
 242:	6a 24       	se_srawi r4,2
 244:	6a 14       	se_srawi r4,1
 246:	7c 84 01 95 	addze.  r4,r4
 24a:	7a 12 00 16 	e_beq   0x260
 24e:	70 c0 e0 00 	e_lis   r6,0
 252:	1c c6 00 00 	e_add16i r6,r6,0
 256:	2a 06       	se_cmpi r6,0
 258:	7a 12 00 08 	e_beq   0x260
 25c:	00 b6       	se_mtctr r6
 25e:	00 06       	se_bctr
 260:	00 04       	se_blr
 262:	44 00       	se_nop
 264:	18 21 06 e8 	e_stwu  r1,-24(r1)
 268:	00 80       	se_mflr r0
 26a:	1b 81 09 08 	e_stmw  r28,8(r1)
 26e:	d7 01       	se_stw  r0,28(r1)
 270:	73 88 e0 03 	e_lis   r28,16387
 274:	30 fc 04 44 	e_lbz   r7,1092(r28)
 278:	2a 07       	se_cmpi r7,0
 27a:	7a 02 00 6a 	e_bne   0x2e4
 27e:	73 e8 e0 03 	e_lis   r31,16387
 282:	73 a8 e0 03 	e_lis   r29,16387
 286:	50 ff 04 48 	e_lwz   r7,1096(r31)
 28a:	1f bd 04 20 	e_add16i r29,r29,1056
 28e:	70 c8 e0 03 	e_lis   r6,16387
 292:	1f ff 04 48 	e_add16i r31,r31,1096
 296:	1f c6 04 24 	e_add16i r30,r6,1060
 29a:	06 de       	se_sub  r30,r29
 29c:	6a 2e       	se_srawi r30,2
 29e:	1b de 84 ff 	e_addi  r30,r30,-1
 2a2:	0d e7       	se_cmpl r7,r30
 2a4:	7a 00 00 1c 	e_bge   0x2c0
 2a8:	20 07       	se_addi r7,1
 2aa:	7c e6 10 70 	e_slwi  r6,r7,2
 2ae:	d0 7f       	se_stw  r7,0(r31)
 2b0:	7c fd 30 2e 	lwzx    r7,r29,r6
 2b4:	00 b7       	se_mtctr r7
 2b6:	00 07       	se_bctrl
 2b8:	c0 7f       	se_lwz  r7,0(r31)
 2ba:	0d e7       	se_cmpl r7,r30
 2bc:	7a 10 ff ec 	e_blt   0x2a8
 2c0:	79 ff ff 41 	e_bl    0x200
 2c4:	70 e0 e0 00 	e_lis   r7,0
 2c8:	1c e7 00 00 	e_add16i r7,r7,0
 2cc:	2a 07       	se_cmpi r7,0
 2ce:	7a 12 00 10 	e_beq   0x2de
 2d2:	70 68 e0 03 	e_lis   r3,16387
 2d6:	1c 63 04 3c 	e_add16i r3,r3,1084
 2da:	79 fc fd 27 	e_bl    0xfffd0000
 2de:	48 17       	se_li   r7,1
 2e0:	34 fc 04 44 	e_stb   r7,1092(r28)
 2e4:	c7 01       	se_lwz  r0,28(r1)
 2e6:	00 90       	se_mtlr r0
 2e8:	1b 81 08 08 	e_lmw   r28,8(r1)
 2ec:	21 71       	se_addi r1,24
 2ee:	00 04       	se_blr
 2f0:	18 21 06 f8 	e_stwu  r1,-8(r1)
 2f4:	00 80       	se_mflr r0
 2f6:	d3 01       	se_stw  r0,12(r1)
 2f8:	c3 01       	se_lwz  r0,12(r1)
 2fa:	00 90       	se_mtlr r0
 2fc:	20 71       	se_addi r1,8
 2fe:	00 04       	se_blr
 300:	18 21 06 f8 	e_stwu  r1,-8(r1)
 304:	00 80       	se_mflr r0
 306:	70 e0 e0 00 	e_lis   r7,0
 30a:	d3 01       	se_stw  r0,12(r1)
 30c:	1c e7 00 00 	e_add16i r7,r7,0
 310:	2a 07       	se_cmpi r7,0
 312:	7a 12 00 18 	e_beq   0x32a
 316:	70 68 e0 03 	e_lis   r3,16387
 31a:	70 88 e0 03 	e_lis   r4,16387
 31e:	1c 63 04 3c 	e_add16i r3,r3,1084
 322:	1c 84 04 4c 	e_add16i r4,r4,1100
 326:	79 fc fc db 	e_bl    0xfffd0000
 32a:	70 e8 e0 03 	e_lis   r7,16387
 32e:	1c e7 04 44 	e_add16i r7,r7,1092
 332:	01 73       	se_mr   r3,r7
 334:	c0 73       	se_lwz  r7,0(r3)
 336:	2a 07       	se_cmpi r7,0
 338:	7a 02 00 0e 	e_bne   0x346
 33c:	c3 01       	se_lwz  r0,12(r1)
 33e:	00 90       	se_mtlr r0
 340:	20 71       	se_addi r1,8
 342:	79 ff fe ee 	e_b     0x230
 346:	70 e0 e0 00 	e_lis   r7,0
 34a:	1c e7 00 00 	e_add16i r7,r7,0
 34e:	2a 07       	se_cmpi r7,0
 350:	7a 12 ff ec 	e_beq   0x33c
 354:	00 b7       	se_mtctr r7
 356:	00 07       	se_bctrl
 358:	79 ff ff e4 	e_b     0x33c
 35c:	18 21 06 f8 	e_stwu  r1,-8(r1)
 360:	00 80       	se_mflr r0
 362:	d3 01       	se_stw  r0,12(r1)
 364:	c3 01       	se_lwz  r0,12(r1)
 366:	00 90       	se_mtlr r0
 368:	20 71       	se_addi r1,8
 36a:	00 04       	se_blr
 36c:	18 21 06 f0 	e_stwu  r1,-16(r1)
 370:	00 80       	se_mflr r0
 372:	d3 f1       	se_stw  r31,12(r1)
 374:	d5 01       	se_stw  r0,20(r1)
 376:	73 e8 e0 03 	e_lis   r31,16387
 37a:	1f ff 04 2c 	e_add16i r31,r31,1068
 37e:	18 ff 02 fc 	e_lwzu  r7,-4(r31)
 382:	73 e7 9f ff 	e_cmp16i r7,-1
 386:	7a 12 00 14 	e_beq   0x39a
 38a:	00 b7       	se_mtctr r7
 38c:	00 07       	se_bctrl
 38e:	18 ff 02 fc 	e_lwzu  r7,-4(r31)
 392:	73 e7 9f ff 	e_cmp16i r7,-1
 396:	7a 02 ff f4 	e_bne   0x38a
 39a:	c5 01       	se_lwz  r0,20(r1)
 39c:	00 90       	se_mtlr r0
 39e:	c3 f1       	se_lwz  r31,12(r1)
 3a0:	20 f1       	se_addi r1,16
 3a2:	00 04       	se_blr
 3a4:	18 21 06 f8 	e_stwu  r1,-8(r1)
 3a8:	00 80       	se_mflr r0
 3aa:	d3 01       	se_stw  r0,12(r1)
 3ac:	c3 01       	se_lwz  r0,12(r1)
 3ae:	00 90       	se_mtlr r0
 3b0:	20 71       	se_addi r1,8
 3b2:	00 04       	se_blr
 3b4:	70 a0 60 00 	e_li    r5,-262144
 3b8:	48 16       	se_li   r6,1
 3ba:	50 e5 02 f8 	e_lwz   r7,760(r5)
 3be:	74 c7 c9 8c 	e_rlwimi r7,r6,25,6,6
 3c2:	54 e5 02 f8 	e_stw   r7,760(r5)
 3c6:	50 e5 13 2c 	e_lwz   r7,4908(r5)
 3ca:	74 e7 c7 ff 	e_rlwinm r7,r7,24,31,31
 3ce:	18 e6 e0 01 	e_xori  r6,r7,1
 3d2:	30 e5 13 2e 	e_lbz   r7,4910(r5)
 3d6:	61 f7       	se_bclri r7,31
 3d8:	44 67       	se_or   r7,r6
 3da:	34 e5 13 2e 	e_stb   r7,4910(r5)
 3de:	00 04       	se_blr
 3e0:	18 21 06 f0 	e_stwu  r1,-16(r1)
 3e4:	00 80       	se_mflr r0
 3e6:	54 01 00 14 	e_stw   r0,20(r1)
 3ea:	79 ff ff 17 	e_bl    0x300
 3ee:	79 ff ff 7f 	e_bl    0x36c
 3f2:	50 01 00 14 	e_lwz   r0,20(r1)
 3f6:	00 90       	se_mtlr r0
 3f8:	20 f1       	se_addi r1,16
 3fa:	00 04       	se_blr
 3fc:	18 21 06 f0 	e_stwu  r1,-16(r1)
 400:	00 80       	se_mflr r0
 402:	54 01 00 14 	e_stw   r0,20(r1)
 406:	79 ff fe 5f 	e_bl    0x264
 40a:	50 01 00 14 	e_lwz   r0,20(r1)
 40e:	00 90       	se_mtlr r0
 410:	20 f1       	se_addi r1,16
 412:	00 04       	se_blr
	...
 420:	ff ff ff ff 	.long 0xffffffff
 424:	00 00       	se_illegal
 426:	00 00       	se_illegal
 428:	ff ff ff ff 	.long 0xffffffff
 42c:	00 00       	se_illegal
 42e:	00 00       	se_illegal
 430:	01 1b       	se_mr   r27,r1
 432:	03 3b       	se_mfar r27,r11
 434:	00 00       	se_illegal
 436:	00 08       	se_rfi
	...
