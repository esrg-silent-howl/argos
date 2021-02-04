# draw rectangle:
#	- stroke:
#		green #00ff00 alpha=1.0
#		2
#	- fill:
#		transparent
#	- position (counting from the top-left):
#		from x=65, y=50
#		to x=300, y=200
#   - draw onto image pf0.jpg and save in pf1.jpg
convert pf0.jpg -strokewidth 2 -stroke "rgba( 0, 255, 0 , 1.0 )" -fill "rgba( 0, 255, 0 , 0 )" -draw "rectangle 65,50 300,200 " pf1.jpg && 
# draw rectangle:
#	- no stroke
#	- fill:
#		white #ffffff alpha=1.0
#	- position (counting from the top-left):
#		from x=65, y=50
#		to x=120, y=30
#   - draw onto image pf1.jpg and save in pf2.jpg
convert pf1.jpg -strokewidth 0 -fill "rgba( 255, 255, 255 , 1 )" -draw "rectangle 65,50 120,30 " pf2.jpg && 
# draw text:
#	- weight (thickness):
#		25
#	- pointsize (actual size)
#		20
#	- position (counting from the top-left (Northwest)):
#		x=70, y=35
#	- text
#		"gun: 90%"
#   - draw onto image pf2.jpg and save in pf3.jpg
convert pf2.jpg -gravity Northwest -weight 25 -pointsize 20 -annotate +70+35 "gun: 90%" pf3.jpg &&
# remove pf1.jpg and pf2.jpg
rm pf1.jpg pf2.jpg