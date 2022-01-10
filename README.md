# CopyCurve
_By ItJustWorks<sup>TM</sup>_

### Build dependencies

- C++ compiler with support for C++11
- CMake >= 3.17
- Git
- Valgrind (optional)

### Build instructions

#### Checkout repository

```shell
git clone git@git.chalmers.se:courses/dit638/students/2021-group-09.git CopyCurve
cd CopyCurve
```

#### Run build

```shell
cmake -P CMake/Scripts/DoBuild.cmake
```

#### Run testsuite

```shell
cmake -P CMake/Scripts/RunTestsuite.cmake
```

### Git Workflow 
1. Each team member will work on their assigned task within their own branch. Before beginning a new task, run the command:  

```shell
git checkout -b <new_branch>
```  

from the latest version of master or whichever branch they wish to continue working towards.  

2. All commit messages should follow the following format:  
`[Tag] <descriptive commit message>`  
The tag helps define how the commit contributes to the project. For example, [CI], [Bug], [Feature], [Docs], [Tests], etc. 

3. After completing a feature/task/implementation, one should create a merge request for their own branch. A merge request must be approved by at least one reviewer and the CI must pass. 

### Code Review Process
Code review occurs mainly during merge requests and [draft merge requests](https://docs.gitlab.com/ee/user/project/merge_requests/work_in_progress_merge_requests.html). At least one team member who has not committed to the merge request must review and approve the merge request in order for it to be merged. Draft merge requests are not ready to be merged, but the author would like to receive feedback on the work they have done up to that point. This is helpful for larger tasks where work is being done incrementally and regular feedback can help the author.

In the reviewing process, one must cover the correctness of the code, test coverage, functionality changes, and confirm that they follow the coding guides and best practices. The reviewer will point out obvious improvements, such as hard to understand code, unclear names, commented out code, untested code, or unhandled edge cases. The reviewer can also note maintainability observations, such as complex logic that could be simplified, improving test structure, removing duplicates, and other possible improvements.

The reviewer may ask open-ended questions and if something is unclear, they can ask for a clarification instead of a correction. Reviewers can also offer alternatives and possible workarounds that might work better for the situation without insisting those solutions are the best or only way to proceed. Code reviews can also be kind and unassuming, they applaud nice solutions and are all-round positive.

Reviewers must not merge merge requests while there are open-ended questions, even if it has been approved by another reviewer. If a comment or a question needs to be addressed by the author, there should be sufficient time given to make the changes or to resolve the comments/questions. For changes that are more urgent than others, reviewers try to make themselves available for quicker reviews.
