import matplotlib.pyplot as plt

# # To get specific plot remove comments of that specific part.
# # By default plot will be generated for zeta-2 = 0%  

# # For zeta-2 = 0 %
c1_values = [30, 40, 50]
c2_values = ["MPU_overall", "MPU Adv1","MPU Adv2" ,"R1"]
results_for_c2 = {
    "MPU_overall": [0.75, 0.619, 0.588],
    "MPU Adv1": [0.5, 0.444, 1],
    "MPU Adv2": [0, 0, 0],
    "R1": [0.167, 0.3, 0.7]
}
x_label = 'Zeta-1 values'
title = 'Results for Zeta-2 = 0%'

# # For zeta-2 = 30 %
# c1_values = [30, 40, 50]
# c2_values = ["MPU_overall", "MPU Adv1","MPU Adv2" ,"R1"]
# results_for_c2 = {
#     "MPU_overall": [0.61, 0.588, 0.5],
#     "MPU Adv1": [0.4,  0.833, 1],
#     "MPU Adv2": [0.66, 0.25, 0],
#     "R1": [0.182, 0.7, 0.723]
# }
# x_label = 'Zeta-1 values'
# title = 'Results for Zeta-2 = 30%'

# # For zeta-2 = 45 %
# c1_values = [30, 40, 50]
# c2_values = ["MPU_overall", "MPU Adv1","MPU Adv2" ,"R1"]
# results_for_c2 = {
#     "MPU_overall": [0.583, 0.56, 0.82],
#     "MPU Adv1": [0,  0, 1],
#     "MPU Adv2": [1, 1, 0],
#     "R1": [0, 0, 0.965]
# }
# x_label = 'Zeta-1 values'
# title = 'Results for Zeta-2 = 45%'

# # For zeta-1 = 30 %
# c1_values = [0, 30, 45]
# c2_values = ["MPU_overall", "MPU Adv1","MPU Adv2" ,"R2"]
# results_for_c2 = {
#     "MPU_overall": [0.75, 0.61, 0.583],
#     "MPU Adv1": [0.5,  0.4, 0],
#     "MPU Adv2": [0, 0.66, 1],
#     "R2": [0, 0.182, 0.92]
# }
# x_label = 'Zeta-2 values'
# title = 'Results for Zeta-1 = 30%'

# # For zeta-1 = 40 %
# c1_values = [0, 30, 45]
# c2_values = ["MPU_overall", "MPU Adv1","MPU Adv2" ,"R2"]
# results_for_c2 = {
#     "MPU_overall": [0.619, 0.588, 0.56],
#     "MPU Adv1": [0.444,  0.833, 0],
#     "MPU Adv2": [0, 0.25, 1],
#     "R2": [0, 0.1, 0.95]
# }
# x_label = 'Zeta-2 values'
# title = 'Results for Zeta-2 = 40%'

# # For zeta-1 = 50 %
# c1_values = [0, 30, 45]
# c2_values = ["MPU_overall", "MPU Adv1","MPU Adv2" ,"R2"]
# results_for_c2 = {
#     "MPU_overall": [0.588, 0.5, 0.82],
#     "MPU Adv1": [1,  1, 1],
#     "MPU Adv2": [0, 0, 0],
#     "R2": [0, 0, 0]
# }
# x_label = 'Zeta-2 values'
# title = 'Results for Zeta-2 = 50%'

# Plotting
for c2 in c2_values:
    plt.plot(c1_values, results_for_c2[c2], marker='o', linestyle='-', label=f'{c2}', markersize=8) 

plt.xlabel(x_label)
plt.ylabel('Results')
plt.title(title)
plt.legend()

# Show plot
plt.grid(True)
plt.show()
