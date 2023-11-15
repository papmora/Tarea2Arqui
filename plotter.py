import os
import glob
from plotly.subplots import make_subplots
import plotly.graph_objects as go
import re
import plotly.io as pio

def plotter():
    files       =  [txt for txt in glob.glob("files/*.txt")]
    # Log file format
    # num_threads = 4 computed pi = 3.141592653589683 in 0.071467046000180 seconds threads =  4 % error = -0.000000000003421
    pattern     =  re.compile("numbers\s+=\s+(\d+).+in\s+(\d+.\d+)")
    total_tests = float(len(files))
    acc_runtimes = []
    acc_speedups   = []
    for txt in files:
        with open(txt) as fd:
            # get both time and thread lists
            nums, runtimes  = zip(*[(float(match.group(1)), float(match.group(2))) for line in fd.readlines() for match in [pattern.search(line)] if match])
            
            # speedup T(1)/Tn
            speed_ups = [runtimes[0]/runt for runt in runtimes]
                
            # first iteration acc and error are empty
            if len(acc_runtimes) == 0:
                acc_runtimes = [0.0]*len(runtimes)
                acc_speedups = [0.0]*len(runtimes)

            # add previous runtime
            acc_runtimes = [sum(runt) for runt in zip(runtimes, acc_runtimes)]

            # add previous speedup
            acc_speedups = [sum(speedup) for speedup in zip(speed_ups, acc_speedups)] 

    
    average_runtimes = [acc_time/total_tests for acc_time in acc_runtimes]
    average_speedups = [acc_speedup/total_tests for acc_speedup in acc_speedups]

    fig = make_subplots(rows=2, cols=1, subplot_titles=("Elements vs Avg Runtimes", 'Elements vs Speedup'))

    fig.add_trace(
        go.Scatter(x=nums, y=average_runtimes),
        row=1, col=1
    )

    fig.add_trace(
        go.Scatter(x=nums, y=average_speedups),
        row=2, col=1
    )

    # edit axis labels
    fig['layout']['xaxis']['title']='Number of Elements'
    fig['layout']['xaxis2']['title']='Number of Elements'
    fig['layout']['yaxis']['title']='Average Runtime (s)'
    fig['layout']['yaxis2']['title']='Average Speedup'
    fig.show()

    pio.write_html(fig, 'plot.html', auto_open=False)
if __name__ == "__main__":
    plotter()