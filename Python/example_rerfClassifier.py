from rerfClassifier import rerfClassifier
from sklearn.datasets import make_classification

X, y = make_classification(
    n_samples=1000,
    n_features=4,
    n_informative=2,
    n_redundant=0,
    random_state=0,
    shuffle=False,
)

clf = rerfClassifier(n_estimators=100, max_depth=2, random_state=0)
clf.fit(X, y)

print(clf.predict([0, 0, 0, 0]))
print(clf.predict_proba([0, 0, 0, 0]))
print(clf)

from sklearn.utils.estimator_checks import check_estimator

check_estimator(rerfClassifier)
