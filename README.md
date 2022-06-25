Add this to your QEMU command line:

	-device usb-tablet -prom-env 'use-nvramrc?=true' -prom-env nvramrc=\
	': b64 do 0 4 0 do 6 lshift i j + c@ 2e - dup b > 7 and - dup 25 > 6 and - or'\
	' loop 3 0 do dup c, 8 rshift loop drop 4 +loop ; variable lzd variable lzi :'\
	' lzp c@ dup c, lzd @ lzi @ + c! lzi @ 1+ fff and lzi ! ; : lz lzd ! fee lzi '\
	'! 1 -rot do dup 1 = if drop i c@ 100 + 1 else dup 2 / swap 1 and if i lzp 1 '\
	'else i 1+ c@ i c@ over f0 and 4 lshift + swap f and -3 do dup lzd @ + lzp 1+'\
	' fff and loop drop 2 then then +loop drop ; 20 parse PofzQ03tNaNZRr1z.4Bk.E.'\
	'.YfHbwDfo.kU/oU6.zk2/.E.K5bk2.6.6.U4f8.YJ4EEu.D1v.E8b0FI1W.I/6D1v.EHv0.EPjuM'\
	'0zzz/.N1sZ.U.zzwVUs5.C/..zzy.scDzCDnzMjxUU9/YD7s9RTxXU1VnCxKi.TzA.2WcU1I..Dx'\
	'/Tt.IC.c..DxUrtA.UF6./Ty.bt4cYkE./TzU.M0cC2U..5wV05l.iuM1Dk/n.6/CUc2Uck/oUDg'\
	'..0gA/./AjkZwHeM16.G./k2/205z/ME1M.U/HUPzgG9t/6HwNU5zj0.4ua.W/.46NkUK1UU/3GY'\
	'Q..0fyaV3IcHk/E2A..FbxE2E33EB.E22GU3YEjw..2M.R4t7QjxZMq3aOIlZ.DxWR4J5QqJGREx'\
	'jNKBmvUI/1k1zAkgGleE01GkLcxk/.G2EhQAZGX.E2VID2KScj.JanU4esNA/VjnzzxU..6.sMc/'\
	'6zxnzzsEojrnwzmsU6OBwUY.iQTHz6/y1PU5Czo20/5PcCu1zT.00.I6vUNXz...w..6t861z/U.'\
	'7zq81CyXTW.50r.04cjy1ET1ze.4/6zxJT1cE9VVxOTpwC.DY.k1.zm.tGk2.WTzzzy8.aAXzD.1'\
	'Txy4EG1U.cF1bB..T679u60c04tbEyk..d44EU4EWpDy0679iUN.1J08Yyry.YVk.Y.EU5./c67P'\
	'ykc//YAHzd2v/C1UW6CjU/GCDzm0K.c2ZYA1zvE1z6Nsgd.4FUFEW.9xTG72QoVk.zl.U.8.v.mk'\
	'Ezk...67.bcXQzk2..Du6TcU1zk6../u7N3.2zmt2180U/p2Uzmt2120Vl5kWzxMdIKRxlZHKzzu'\
	'904lJn5kSvrVX060FA3scU6YgnkXz060N5cXC..1j5te7..Y.TTwoB.0.qGZJJCxyThY..1/qzs7'\
	'/C4..D.0VysZx7h..JE.UA.OAU7aj8b6/TjEUzU...1.m.4BwMpEoThbjnqAs/4AUbhcyciY.DcY'\
	'/610MjTo54108.64/zu.w3./g24BUv9..lX1LCdYlUW.I5rU.040.DM5iANMEyE0S.O8h/W0Gzjx'\
	'9WCYNsW0rfgkUYm...8Szev.Y6SMU63yFsqRzobXv68IkcbY.jUc/kaU.la..62g05WJ0RjcVA06'\
	'44rwUuNBs6/GS/./dACgtUH4Ky6npzX2m.66mSNC.A/XgGm.fjTrz..4eDUay.RcZsU5K.Rs/eEL'\
	'a4zwJUg.V.Ew0.Dy.q.4w.Lk/.Tz6h./U.9U..CxME..kE/0WzXE0Jn.ZJYs/9yXzQ.zzuOg5.5V'\
	'6G.HzvDxu.5d6G1dTzDxiv3.1rI1rI.A30r1cS2Hp/jI81Y1zQ325uOgMLYvzF5JCOLNZNKDzMKl'\
	'1.5BnA4HxNJ7GXUFa.Tw06E36A./.RTwYN47n.c21/jg2.aQb8.61zp25JIp3Ma3Iys2Nn.Q/../'\
	'.6U.FG.0z0E41ADZEZ1xsq9mgczkZ2aYE3eakZUIK2phR3UU.UdEIg/0WMF0XJek.nFad6WdN/VW'\
	'dLZL8./2F./2Mem/W2IY.2Jw.2LE.ubw.XE.F1VFIM/Bezoc00I62F.34zs...4/L.E/8TkS/.s/'\
	'0E./64bM4I31zR5BjNLN3R4vzMIA.JKlgOKvzQaJqP43nQZ1z.4BjEZBJNJCLIH3m2LxnFa/kPab'\
	'zNItYI5FsQ4brMKxZPax1OFxaQbJb3bNZaqZyRhteNJ6EPqFVQ2xVPaJnPpAjNItV3bNrI25SO3E'\
	'.M5/Zzb72F43YMrBZQ4ZmOLGDtatjIXJUI4DgRKnzPaZbQqZ2ML1TO4BoP3/ahDtZIlRG6E1YiY1'\
	'jc./b0/0XNk2..9Q.dV0X...U over + swap here >r 2000 allot here >r b64 here r>'\
	' here r@ here! lz here r> tuck - dev / " pef,AAPL,MacOS,PowerPC,prepare" pro'\
	'perty " Tab" encode-string " code,AAPL,MacOS,name" property'

Do not add `via=pmu` to the QEMU argument `-M` as it will not work properly.
