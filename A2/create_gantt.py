# read from the output file and create gantt charts for each algo and save them 
# read output.txt
# create ALGO_GANTT.png -> EXP_0_FCFS_GANTT.png 
import sys
import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np

def create_dict(line):
    pro_dict ={}
    tokens = line.split()
    num_tokens = len(tokens)
    num_bursts = num_tokens/3
    for i in range(0,num_tokens,3):
        PID  = tokens[i]
        start = float(tokens[i+1])
        end = float(tokens[i+2])
        if(PID in pro_dict):
            pro_dict[PID][0].append(start)
            pro_dict[PID][1].append(end)
        else:
            pro_dict[PID] = [[start],[end]]
    return pro_dict


def create_ls(pro_dict):
    pro_ls = []
    for key in pro_dict:
        pro_ls.append((key,pro_dict[key][0],pro_dict[key][1]))

    pro_ls.sort()
    return pro_ls


def create_gantt(line,algo_name,prefix):
    process_dict = create_dict(line) ## key is the PID and the value is a tuple lists of start and end times 
    process_ls = create_ls(process_dict)
    fig,ax = plt.subplots()
    ls_PID = []
    name_fig = prefix+"_"+algo_name+".png"
    for idx in range(len(process_ls)):
         num_bursts = len(process_ls[idx][1])
         x_pos = [idx for i in range(num_bursts)]
         ls_PID.append(process_ls[idx][0])
         ax.vlines(x_pos,process_ls[idx][1],process_ls[idx][2])
    ax.set_title(prefix+"_"+algo_name)
    ax.set_xlabel("Jobs")
    ax.set_ylabel("Time in ms")
    ax.set_xticks([i for i in range(len(process_ls))],labels=ls_PID)
    plt.savefig(name_fig)
    return 

def main():
    input_fn = sys.argv[1] 
    exp_prefix = sys.argv[2]
    name_algos = ["FCFS","RR","SJF","SCTF","MLFQ"]
    with open(input_fn,"r") as fh:
        lines = fh.readlines()
        for line_idx in range(0,10,2):
            line = lines[line_idx].rstrip()
            algo_idx = line_idx//2
            create_gantt(line,name_algos[algo_idx],exp_prefix)
    return 

if __name__=="__main__":
    main()

