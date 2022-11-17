import os
import pathlib
import subprocess
import sys

#Compares result files with files in "compare" folder
#TODO if artiface passed, move it to separate folder

class pCodes:
    PROCESS_FAILED = 1

def FetchSourcesPath(dirPath) -> list:
    paths = []

    for root, dirs, names in os.walk(dirPath):
        for name in names:
            if name.endswith(".h"):
                paths.append(f"{os.path.join(dirPath, name)}")
    return paths

def FindArtifacts() -> list:
    arts = []

    path = pathlib.Path(os.path.realpath(__file__)).parent.parent.parent.joinpath("artifacts") #TODO

    if not path.exists():
        print(f"Cant find artifacts folder")
        sys.exit(1)

    for root, dirs, names in os.walk(path):
        for name in names:
            if name.endswith(".exe"):
                arts.append(os.path.join(root, name))
    return arts

def RunProcess(args) -> int:
    process = subprocess.run(args)
    return process.returncode

def CheckOutputFile(path: str) -> bool:
    comparePath = pathlib.Path(os.path.realpath(__file__)).parent.joinpath("compare")
    index = path.rfind('\\')
    index += 1
    resultName = path[index : len(path)]
    comparePath = comparePath.joinpath(resultName)
    with open(path, 'r') as f1, open (comparePath, 'r') as f2:
        if f1.readlines() == f2.readlines() : return True
        else: return False

artifacts = FindArtifacts()
filesToTest = FetchSourcesPath(os.path.dirname(os.path.realpath(__file__)))
for filePath in filesToTest:
    for artifactPath in artifacts:
        artifactName = artifactPath[artifactPath.rfind('\\') + 1 : len(artifactPath)]
        outputPath = filePath + "." + artifactName + ".res" # append artifact and res extentions
        pArgs = [artifactPath, filePath, outputPath]
        pCode =  RunProcess(pArgs)
        print("\n") # if process's stdout doesn't write this at the end
        if pCode is None or pCode > 0: 
            print(f"Process {artifactPath} failed with {pCode} code, test file {filePath}")
            continue
        if not CheckOutputFile(outputPath):
            print(f"artifact {artifactPath}, file {filePath}, output {outputPath} FAILED")
        else: print(f"artifact {artifactPath}, file {filePath}, output {outputPath} PASSED")