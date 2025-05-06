READMe

// Project Overview //
This C++ project takes workload export files from Celoxis and Jira. Both are project management tools used in both agile and waterfall setups. I built a program that parses task data (including resource names, task duration, effort, and workspace) to generate a year-long calendar that tracks how staff time is allocated.

The calendar supports filtering by department (Consulting or Development) and by job role. It also adjusts for each person’s labor target using an input file. The output includes monthly utilization percentages and summary flags for milestones, release months, and potential workload risks.


// Project Data Structure Implemented //
The data structure implement in the calendar is a singly linked skip list, where each node represents a month. Each MonthNode tracks:

monthName: "Jan", "Feb", ...
availableHours: calculated using business days
consultingTasks and devTasks: separated by workspace
milestoneCount and isReleaseMonth: flags
next: pointer to the next month

The pointer setup allows tasks that span multiple months to be split evenly. This same application is used in the project management tools, so it's important for our program to reflect the same. If a month’s utilization exceeds 100% the logic shifts the effort to the next available month without affecting fixed dates like milestones or release deadlines. Ideally over time, we would include a priority indicator so that the program moves low priority tasks before high priority tasks. 

// How to Run Program //
When you run the program, you’ll be asked to enter:
    Year - I suggest using 2025 for the most complete dataset
    Workspace - all, consulting, or development
    Role - - I suggest using all. Other roles include: Scientist I, Scientist II. More roles are included in usersinfo.

    The program will then generate a monthly table with utilization percentages and the following indicators:
        [M] – milestone present that month
        [R] – release month (January and July)
        [LOW] – under 70% utilization
        [OK] – between 70% and 90% utilization
        [HIGH] – over 90%, flagged as overallocated