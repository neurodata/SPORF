from tensorflow import keras
from tensorflow.keras import layers

class Resnet50:
    """
    Resnet50 neural network (with scikilearn api wrapping of keras)

    Parameters
    ----------
    data_shape : numpy.ndarray
        Data shape
    n_classes : int
        Number of classes
    batch_size : int (default 32)
        minibatch size during training
    epochs : int (default 100)
        Number of epochs to train for
    """
    def __init__(self, data_shape, n_classes, batch_size=32, epochs=100):
        self.data_shape = data_shape
        self.n_classes = n_classes
        self.batch_size = batch_size
        self.epochs = epochs
        self.model = keras.applications.ResNet50(
            input_shape = data_shape,
            weights=None,
            classes = n_classes
            )
        self.model.compile(loss="categorical_crossentropy", optimizer="adam", metrics=["accuracy"])


    def fit(self, X, y):
        assert self.n_classes == len(set(y))
        X = X.reshape(-1, *self.data_shape)
        y = keras.utils.to_categorical(y, self.n_classes)
        self.model.fit(X, y, batch_size=self.batch_size, epochs=self.epochs)

    def predict_proba(self, X):
        X = X.reshape(-1, *self.data_shape)
        y_prob = self.model.predict(X) 
        return y_prob

    def predict(self, X):
        X = X.reshape(-1, *self.data_shape)
        y_prob = self.model.predict(X) 
        y_classes = y_prob.argmax(axis=-1)

        return y_classes
    
    
        