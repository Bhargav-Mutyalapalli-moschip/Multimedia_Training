import subprocess



#resolution data extraction
def resolution_extraction():
    fd = open("my_test.lst","r")
    if(not fd):
        print("Failed the open resolutions input file")
    else:
        print("Successfully opened resolutions input file")

    data = fd.readlines()

    my_list = []

    for line in data:

        try:
            each_line = line.strip().split(",")
            width,height = map(int,each_line)
            my_list.append({"width":width,"height":height})
        except ValueError:
                print(f"Skipping invalid line: {line}")

    return my_list


def pipeline_construction(wid,hght):

    for caps in resolutions:
        pipeline = f"gst-launch-1.0 videotestsrc ! video/x-raw,width={wid},height={hght},format=YV12 ! fakesink"

        print(pipeline)
        return pipeline

def pipeline_running(pipeline):
    # Run the command and capture output and error
    try:
        result = subprocess.run(
            f'timeout 10 {pipeline}',  # Use timeout to limit execution time
            shell=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )

        # Check if the process exited successfully
        print("code:",result.returncode)
        if result.returncode != 1 and result.stderr.strip() == '':
            return "Pass"
        else:
            return f"Fail - Error: {result.stderr.strip()}"

    except subprocess.CalledProcessError as e:
        return f"Fail - Error: {e.stderr.strip()}"
    except subprocess.TimeoutExpired:
        return "Fail - Timeout"

def report_creation(result,pipe):
    fd = open("results.txt","a")
    fd.write(f"Pipeline:{pipe},Results is :{result}\n")
    fd.write("------------------------------------------------------------------------------------------------------------\n")
    return 0

if __name__ == "__main__":
    fd = open("results.txt","w")
    if(not fd):
        print("Failed to create the results file")
    else:
        print("Successfully created results file")

    resolutions = resolution_extraction()

    for caps in resolutions:
        print(caps)

        gen_pipeline = pipeline_construction(caps['width'],caps['height'])

        report = pipeline_running(gen_pipeline)

        updation = report_creation(report,gen_pipeline)

        if(updation == 0):
            print("Succesfully updated info")
        else:
           print("Failed to  update info")

