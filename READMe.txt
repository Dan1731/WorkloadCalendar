READMe

// Project Overview //
This C++ project uses a workload export file from Celoxis and Jira. Both are project management tools utilized to track projects and tasks in agile and waterfall methods. I have put together a report that pulls resource names, duration of task, effort, and workspace to build a yearly calendar that analyzes how staff time is allocated. This program will support filtering among departments both consulting and development as well as job role, while adjusting for individual labor targets. This project calculates monthly utilization percentages and includes summaries of each month whether there are milestones, release dates or workload risks.


// Project Data Structure Implemented //
The goal of the calendar was to implement using a single linked skip list. Within the list, each node represents a month called monthName and tracks availableHours (in business days), consultingTaks and devTasks (workspaces), milestoneCount and isReleaseMonth (flags), and next (pointer to the following month).

The purpose of the pointer so that if our utilization is over 100% within a month, the pointer will shift hours into the next monthName. This allows projects to be worked over months to not interfere with milestones and releases.

// How to Run Program //
The program will prompt the user three questions:
    Enter year
    Workspace
    Role

Celoxis is a newly implemented management tool for our company. So years before 2025 lack information. I recommend using 2025 to capture more accurate information. This program will then allow us to track further years better. 

Workspace - you can either select all, consulting, or development depending on what you would like to track.

Role - Scientist I, Scientist II, All. Other roles if you want to search are included in usersinfo.

The program will then output a table with the desired information. 

[M] is for milestones
[R] is for releases (Jan and July)

[LOW], [OK], [HIGH] are risk indicators based on total utilization. 
