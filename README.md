Add this to your QEMU command line:

	-device usb-tablet -prom-env 'use-nvramrc?=true' -prom-env nvramrc=\
	': b64 do 0 4 0 do 6 lshift i j + c@ 2e - dup b > 7 and - dup 25 > 6 and - or'\
	' loop 3 0 do dup c, 8 rshift loop drop 4 +loop ; variable lzd variable lzi :'\
	' lzp c@ dup c, lzd @ lzi @ + c! lzi @ 1+ fff and lzi ! ; : lz lzd ! fee lzi '\
	'! 1 -rot do dup 1 = if drop i c@ 100 + 1 else dup 2 / swap 1 and if i lzp 1 '\
	'else i 1+ c@ i c@ over f0 and 4 lshift + swap f and -3 do dup lzd @ + lzp 1+'\
	' fff and loop drop 2 then then +loop drop ; 20 parse PofzQ03tNaNZRr1z.4Bk.E.'\
	'.YfHbwDfo.kU/oU6.zk2/.E.K5ik2.6.6.U4fA.YJ4EEuQD1v.E8b0FI1Y.I/YD1v.EHvT.A/dU6'\
	'6cTyzYDHz0../6TyICv1zzzz.TzxwgrUPbUj.MDwsC4FWpOu.MzwwG5BpZE..ETy.U/E.fU6.Thx'\
	'wULUP.4mSzk.AU2/.C.k.zTxU.7PCM1VUClU..81L.7MM.Ne/zk6tTkk.dUCdzmAtCTzzzzw6zkW'\
	'Ab.2..E.7zk/0UTXz2..zzk.tDG3tPoc6zsa2T.E..2.2zs7.YzXz0U11zmCFU/6.e.KUjuCEY.E'\
	'.e.JUU.4m..5z6HVMT3...kXjQviaU2s.68g..Di/.NiE.0gAx5tA.kY.U2ua2GY29jHh6.ET0k2'\
	'/zlUV/ME5M.k/zos4gG9t/6HwDqM/j0.4ua.W3UE/Nu66TV./.F2QN.3bPD1uwDd0138A2EI/3A2'\
	'0./2BW/HR...G.E22zk3YG.38.2M.zqt7QaJoMq3azolZ.47dRqJCzrFEFk/mIbFZ5rBZQbJj.KJ'\
	'Xzys0NUw..UgG7nuU.Eog6/d24i0J9JUUGW.U2cID9N7QNc.U.9c32GM.EE7iuDzvc.Fqzy81CTb'\
	'gtW.PClU.4.300GmzCk...9m0.Pz0UY26hWU.fk2U.C.s.9s06A61hgd2kU6UhaUU6R2UxoVcU.2'\
	'.G.0uzqAoTDnz9VWQ4NxwyWvxCDE..E0UsxxzY5Xv.Q0zP8o/tU4OCpEUBm1f6T/vxRXpyRYUzt.'\
	'WzVU.YH209..T.9yPtXnt.tYU5..TjlmEU/k.2./z21.K.mnr.DY.WBU..DySzcU/W.A..Dxy5cY'\
	'0I.E.FDxYc8.iIG.AFDw5682iT06A8Tz2FrrKJBN7Wzz4GpLyT/s6KvzAU73sXWU6zmkk0607U7b'\
	'Djws6./u6acY.0k/gB.1r6/cj.6/xJTwoThZgqE/IejdykU..E4.U1.0O61ajZc..lUU.zk.k.G0'\
	'NcHVmvXU..1/WP6.s.4A.J9woThZX2qAsuqAkZXs2ajI.DcY/.Bw./m/J6A8xzs2MC.0/.4mUnq.'\
	'I.//XE.ikaH5u8osu04..ARviqE0Kek4O42.vw3ckxR.Vrh2UU09tzDyWW120.DT3/tA.rtAkzm.'\
	'..5yHrv.YiXE.iE0K.E8O4aU.g4.Eal6S7N6Uk.Fq69gtsjy.YDnz5.1AdBy6a.6..0ibbzxWzoj'\
	'gXjbx2/MV4d765Y.E30MF5oPYs/7.HvcYq/7.E2zGuhkG2Y/D.iXx6.FTu02L.fS0y.29A.2.00/'\
	'U8.0v80.ODik/jmBF.IU..k4C.J.z/6DzQrI/UKFW/.9jNmQ4.UA0/zwcHIJFMJFJ4jxW..2XUU8'\
	'..jw/.V26/1.V.Ey1A1U03.FbLMs5NFGO36M36/e6.H0X5PswLvt470c.3AAe4/HC3U.F..18./3'\
	'73BNTs00r/CwGFUF0.G/Qxc./EU0J4ic/Pp/ERDxnNLN3.5FiMTx1JKlgRaZiQjxZP43nPr7E.Dx'\
	'XEZBJQaJHOQxqQqJXIV5nOINUN4vzS4JCOJ/oNL1vEq3FNatjNsxdNL7pIFPe2CdVNLxG.4FVNKV'\
	'Iyq/GFb72NIFVQaBnQ4azPqZoM77iPCx1QrBVI4CMzrJgPaZbQqZ2hq3kO4BoP3/iI1lZ.Dw.t3A'\
	'Lvm2.Nud.gac.Nk3G4c2..wUU..2. over + swap here >r 2000 allot here >r b64 her'\
	'e r> here r@ here! lz here r> tuck - dev / " pef,AAPL,MacOS,PowerPC,prepare"'\
	' property " Tab" encode-string " code,AAPL,MacOS,name" property'

Do not add `via=pmu` to the QEMU argument `-M` as it will not work properly.
