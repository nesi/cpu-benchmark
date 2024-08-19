# Load required libraries
library(ggplot2)
library(gghighlight)
library(readr)
library(tidyr)

# Read the CSV file
data <- read_csv("average-multiplication.csv")

# Reshape the data from wide to long format
data_long <- pivot_longer(data, cols = c(Genoa, Milan), names_to = "microarchitecture", values_to = "Time")

# Create the line chart
ggplot(data_long, aes(x = ntasks, y = Time, color = microarchitecture)) +
  geom_line() +
  geom_point(size = 3) +
  gghighlight() +
  scale_color_manual(values = c("Genoa" = "blue", "Milan" = "red")) +
  scale_x_continuous(breaks = c(16, 32, 48, 64, 80, 96, 112, 128)) +
  labs(title = "Average runtime per multiplication (s)\nfor MATRIX_SIZE=2048 & NUM_ITERATIONS=100: Genoa vs Milan",
       x = "Number of Threads  ( OMP_NUM_THREADS)",
       y = "Average time per multiplication: (seconds)") +
  theme_minimal() +
  theme(legend.position = "bottom",
        panel.background = element_rect(fill = "gray95", color = NA),
        plot.background = element_rect(fill = "white", color = NA))

