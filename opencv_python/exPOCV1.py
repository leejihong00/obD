import cv2
img_color = cv2.imread('lena.jpg', cv2.IMREAD_COLOR)
cv2.namedWindow('Show Image')
cv2.imshow('Show Image', img_color)
cv2.waitKey(10)

img_gray = cv2.cvtColor(img_color, cv2.COLOR_BGR2GRAY)
cv2.imshow('Show Image', img_gray)
cv2.waitKey(10)

cv2.imwrite('savedimage.jpg', img_gray)
cv2.destroyAllWindows()
