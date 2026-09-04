# online machine learning, one sample at a time
from river import linear_model, preprocessing

model = preprocessing.StandardScaler() | linear_model.LinearRegression()

for x, y in stream:
    model.predict_one(x)   # learn
    model.learn_one(x, y)  # forever

# river-ml.github.io 🌊


# --- option 2 : ultra minimal ---

from river import linear_model

model = linear_model.LinearRegression()

for x, y in stream:
    model.learn_one(x, y)  # never batch again


# --- option 3 : le pipe mis en avant ---

model = (
    preprocessing.StandardScaler()
    | linear_model.LinearRegression()
)  # this is a pipeline. yes, really.


# --- option 4 : anomaly / flux infini ---

from river import anomaly

model = anomaly.HalfSpaceTrees()

while True:
    x = sensor.read()
    if model.score_one(x) > 0.9:
        alert()
    model.learn_one(x)


# --- option 5 : punchline dans le code ---

model = preprocessing.StandardScaler() | linear_model.LinearRegression()

for x, y in stream:      # the stream never stops
    model.learn_one(x, y)  # so neither do we
