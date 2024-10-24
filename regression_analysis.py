# Import necessary libraries
from sklearn.linear_model import LinearRegression
import numpy as np
import matplotlib.pyplot as plt

# Example data for concurrency levels (1 to 20)
concurrency_levels = np.array([i for i in range(1, 21)]).reshape(-1, 1)

# Requests per second data for fork and thread servers
fork_rps = np.array([1287.38, 1979.73, 2012.53, 2107.79, 2049.78, 2053.31, 2052.66, 2046.74, 2059.70, 2051.74, 
                     2067.81, 2062.32, 2055.58, 2069.93, 2073.57, 2091.18, 2092.18, 2058.51, 2068.87, 2068.0]).reshape(-1, 1)
thread_rps = np.array([2344.55, 3924.76, 5298.33, 5848.16, 5934.70, 6124.16, 6300.39, 5779.75, 6339.63, 6549.56, 
                       6939.12, 6275.48, 6315.45, 6341.66, 6375.08, 6246.77, 6165.31, 6195.48, 5409.56, 5400.0]).reshape(-1, 1)

# Create Linear Regression model for both Forked and Threaded servers
fork_model = LinearRegression()
thread_model = LinearRegression()

# Fit the models to the data
fork_model.fit(concurrency_levels, fork_rps)
thread_model.fit(concurrency_levels, thread_rps)

# Predict values for concurrency levels up to 60
future_concurrency = np.array([i for i in range(1, 61)]).reshape(-1, 1)
fork_predictions = fork_model.predict(future_concurrency)
thread_predictions = thread_model.predict(future_concurrency)

# Plotting results for both Forked and Threaded servers
plt.figure(figsize=(10, 6))
plt.plot(concurrency_levels, fork_rps, 'bo', label="Forked Server Actual")
plt.plot(future_concurrency, fork_predictions, 'b-', label="Forked Server Predicted (Linear)")
plt.plot(concurrency_levels, thread_rps, 'go', label="Threaded Server Actual")
plt.plot(future_concurrency, thread_predictions, 'g-', label="Threaded Server Predicted (Linear)")

plt.xlabel('Concurrency Level')
plt.ylabel('Requests per Second')
plt.title('Linear Regression: Forked vs Threaded Server')
plt.legend()

# Save the plot in the current folder with the name 'Linear_Regression_analysis.png'
plt.savefig('Linear_Regression_analysis.png')

# Display the plot
#plt.show()

