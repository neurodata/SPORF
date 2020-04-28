import numpy as np
import rerf
from rerf.rerfClassifier import rerfClassifier

X = np.random.normal(0, 1, size=(10, 10))
y = [1, 1, 1, 1, 1, 0, 0, 0, 0, 0]

clf = rerfClassifier(
    projection_matrix="S-RerF",
    image_height=2,
    image_width=5,
    patch_height_min=1,
    patch_width_min=1,
    patch_width_max=2,
    patch_height_max=2,
    wrap=1,
)

print(clf.fit(X,y))

