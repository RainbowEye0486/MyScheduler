import matplotlib.pyplot as plt
import csv
import re
from matplotlib.pyplot import MultipleLocator

x = []
y = []
sy = []
ty = []

test = 'f'


compare = ""
if test == 't':
    compare = "timelog"
elif test == 'f':
    compare = "fitness"
elif test == 'a':
    compare = "avg_fitness"

  
with open('/home/airobots/myScheduler/data/results/fully_' + compare + '_GP.csv','r') as csvfile:
    lines = csv.reader(csvfile, delimiter=',')
    i = 1
    for row in lines:
        x.append(i)
        # y append the index of the row
        if (test == 't' or test == 'a'):
            string = str(row)
            conv = re.findall(r'\d+\.\d+', string)
            y.append(float(conv[0]))
        else:
            y.append(int(row[0]))
        i += 1


with open('/home/airobots/myScheduler/data/results/fully_' + compare + '_GP-20.csv','r') as csvfile2:
    lines = csv.reader(csvfile2, delimiter=',')
    i = 1
    for row in lines:
        # y append the index of the row
        if (test == 't' or test == 'a'):
            string = str(row)
            conv = re.findall(r'\d+\.\d+', string)
            sy.append(float(conv[0]))
        else:
            sy.append(int(row[0]))

# with open('/home/airobots/myScheduler/data/results/old_' + compare + '_GP-6.csv','r') as csvfile2:
#     lines = csv.reader(csvfile2, delimiter=',')
#     i = 1
#     for row in lines:
#         # y append the index of the row
#         if (test == 't' or test == 'a'):
#             string = str(row)
#             conv = re.findall(r'\d+\.\d+', string)
#             ty.append(float(conv[0]))
#         elif test == 'f':
#             ty.append(int(row[0]))


fig = plt.figure(figsize=(10, 6))



plt.plot(x, y, color = 'g', linestyle = 'solid',marker = '', label = "GP-orig")
plt.plot(x, sy, color = 'r', linestyle = 'solid',marker = '',label = "GP-fully")
# plt.plot(x, ty, color = 'b', linestyle = 'solid',marker = '',label = "200 generations")

# y_major_locator = MultipleLocator(50)
# ay = plt.gca()
# ay.yaxis.set_major_locator(y_major_locator)

# plt.xlim((0, 1400))

plt.xlabel('Data')
plt.ylabel('Makespan')
plt.title('GP Avarage Fitness', fontsize = 12)
plt.legend()
plt.show()