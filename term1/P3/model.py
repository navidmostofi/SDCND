# import libraries
import pandas as pd
import cv2
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.gridspec import GridSpec

# read driving_log.csv and extract image addresses and steering angles
path = 'data/data/'
file = path + 'driving_log.csv'

driving_log = pd.read_csv(file)

imgs_center = driving_log['center']
imgs_left = driving_log['left']
imgs_right = driving_log['right']
steering = driving_log['steering']

center_imgs = []
for item in imgs_center:
    img = plt.imread(path + item)
    center_imgs.append(img)

plt.imshow(center_imgs[0])

left_imgs = []
for item in imgs_left:
    img = plt.imread(path + item[1:])
    left_imgs.append(img)

plt.imshow(left_imgs[0])

right_imgs = []
for item in imgs_right:
    img = plt.imread(path + item[1:])
    right_imgs.append(img)

plt.imshow(right_imgs[0])

center_imgs = np.array(center_imgs)
left_imgs = np.array(left_imgs)
right_imgs = np.array(right_imgs)
steering_angle = np.array(steering)

print('center camera imagse shape:',center_imgs.shape)
print('left camera imagse shape:',left_imgs.shape)
print('right camera imagse shape:',right_imgs.shape)
print('steering angle array shape:',steering_angle.shape)


### helpful functions

# brighness augmentation
def augment_brightness_camera_images(image):
    image1 = cv2.cvtColor(image, cv2.COLOR_RGB2HSV)
    image1 = np.array(image1, dtype=np.float64)
    random_bright = .5 + np.random.uniform()
    image1[:, :, 2] = image1[:, :, 2] * random_bright
    image1[:, :, 2][image1[:, :, 2] > 255] = 255
    image1 = np.array(image1, dtype=np.uint8)
    image1 = cv2.cvtColor(image1, cv2.COLOR_HSV2RGB)
    return image1

# plot images randomely
def plot_imgs(imgs, labels, n_cols, n_rows):
    plt.figure(figsize=(10, 10))
    gs = GridSpec(n_rows, n_cols)
    gs.update(hspace=0.5, wspace=0.1)

    for i in range(n_cols * n_rows):
        ax1 = plt.subplot(gs[i])
        rand_ind = np.random.randint(0, labels.shape[0])
        plt.imshow(imgs[rand_ind])
        plt.text(1, 3, '{0}'.format(labels[rand_ind]), color='k', backgroundcolor='c')
        plt.axis('off')


## data augmentation
augmented_images, augmented_measurements = [], []
correct_factor = 0.25

for image, meas in zip(center_imgs, steering_angle):
    augmented_images.append(image)
    augmented_measurements.append(meas)
    augmented_images.append(cv2.flip(image, 1))
    augmented_measurements.append(meas * (-1.0))
    augmented_images.append(augment_brightness_camera_images(image))
    augmented_measurements.append(meas)

for image, meas in zip(left_imgs, steering_angle):
    augmented_images.append(image)
    augmented_measurements.append(meas + correct_factor)
    augmented_images.append(cv2.flip(image, 1))
    augmented_measurements.append((meas + correct_factor) * (-1.0))
    augmented_images.append(augment_brightness_camera_images(image))
    augmented_measurements.append(meas)

for image, meas in zip(right_imgs, steering_angle):
    augmented_images.append(image)
    augmented_measurements.append(meas - correct_factor)
    augmented_images.append(cv2.flip(image, 1))
    augmented_measurements.append((meas - correct_factor) * (-1.0))
    augmented_images.append(augment_brightness_camera_images(image))
    augmented_measurements.append(meas)


x_train = np.array(augmented_images)
print('x_train shape:', x_train.shape)

y_train = np.array(augmented_measurements)
print('y_train shape:', y_train.shape)

plot_imgs(x_train, y_train, 5, 5)

## plot histogram of training data
plt.hist(y_train, bins=20)


## model architecture
import keras
from keras.layers import Conv2D, MaxPooling2D, Dense, Dropout, Lambda, Flatten, Cropping2D
from keras.models import Sequential
from keras.utils import np_utils
from sklearn.model_selection import train_test_split
from keras.callbacks import ModelCheckpoint

model = Sequential()
model.add(Lambda(lambda x: x/255.0 - 0.5, input_shape=(160,320,3)))
model.add(Cropping2D(cropping=((50,20), (0,0))))
model.add(Conv2D(20, 5, 5, activation='relu', border_mode='same'))
model.add(MaxPooling2D(pool_size=(2,2), strides=(2,2)))
model.add(Conv2D(40, 5, 5, activation='relu', border_mode='same'))
model.add(MaxPooling2D(pool_size=(2,2), strides=(2,2)))
model.add(Conv2D(60, 5, 5, activation='relu', border_mode='same'))
model.add(MaxPooling2D(pool_size=(2,2), strides=(2,2)))
model.add(Conv2D(80, 3, 3, activation='relu', border_mode='same'))
model.add(MaxPooling2D(pool_size=(2,2), strides=(2,2)))
model.add(Dropout(0.5))
model.add(Flatten())
model.add(Dense(512, activation='elu'))
model.add(Dense(256, activation='elu'))
model.add(Dense(128, activation='elu'))
model.add(Dense(1))

model.compile(optimizer='adam', loss='mse')

# create modelcheckpoint to save the best model based on validation loss
filepath='model_weights5.h5'
modelCheckpoint = ModelCheckpoint(filepath,
                                  monitor = 'val_loss',
                                  save_best_only = True,
                                  mode = 'min',
                                  verbose = 1,
                                 save_weights_only = True)

# train the model
model.fit(x_train, y_train, batch_size=64, nb_epoch = 5, shuffle=True, validation_split=0.2, callbacks=[modelCheckpoint])

# save model
model.save('model.h5')