import numpy as np
import rerf
from rerf.rerfClassifier import rerfClassifier

X = np.random.normal(0, 1, size=(10, 10))
X = np.asarray(10*[1,0,0,0,0,1] + 10*[0,0,1,1,0,0]).reshape(20,-1)
y = 10*[0] + 10*[1]

clf = rerfClassifier(
    projection_matrix="S-RerF",
    image_height=1,
    image_width=6,
    patch_height_min=1,
    patch_width_min=1,
    patch_width_max=4,
    patch_height_max=1,
    wrap=0,
)
clf.fit(X,y)
print(sum(clf.predict(X) == np.asarray(y)))

