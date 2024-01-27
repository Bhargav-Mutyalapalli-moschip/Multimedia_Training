import subprocess
Lines = 3

#Reading the lst file 
def read_lst_file(file_path):
    # Read configurations from the lst file
    with open(file_path, 'r') as file:
        lines = file.readlines()

    # Parse configurations from each line
    configurations = []
    for line in lines:
        values = line.strip().split(',')
        print("values:",values)
        if len(values) == Lines:
            try:
                width, height = map(int, values[:2])
                formate = values[2]
                configurations.append({'width': width, 'height': height,'format':formate})
            except ValueError:
                print(f"Skipping invalid line: {line}")
        else:
            print("The lines are read and arranged in a configurations list\n")

    return configurations

#-----------------------------------------------------------------------------------------#


def gstreamer_pipeline_generation(width, height,formate):
    # Construct GStreamer pipeline command based on the configuration
    pipeline_cmd = (
        f'gst-launch-1.0 videotestsrc ! '
        f'video/x-raw,width={width},height={height},format={formate} ! '
        f'autovideosink'
    )
    return pipeline_cmd


#-------------------------------------------------------------------------------------------#
def run_pipeline(pipeline_command):
    # Run the command and capture output and error
    try:
        result = subprocess.run(
            f'timeout 10 {pipeline_command}',  # Use timeout to limit execution time
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


#----------------------------------------------------------------------------------------------#

def report_creation(report,width,height,formate):
    with open("report.txt", "a") as report_file:
            report_file.write(f"\nResolution: {width}x{height} for format {formate}, Result: {report}\n")


#----------------------------------------------------------------------------------------------#

#Path for lst file is assigned
if __name__ == "__main__":
    lst_file_path = "/home/bhargav/Downloads/Scripts/With_LST/my_test.lst"
    configurations = read_lst_file(lst_file_path)
    print("config",configurations)
    #It creates the file if it is not existed
    fd=open("report.txt","w")
    if(not fd):
        print("File is not created\n")
    else:
        print("File is created successfully\n")



    for caps in configurations:
        #It generates the pipeline with the desired caps
        pipeline = gstreamer_pipeline_generation(caps['width'],caps['height'],caps['format'])
        #It runs the pipeline with assigned caps
        result = run_pipeline(pipeline)
        #It created the report according to pieline testing
        report_creation(result,caps['width'],caps['height'],caps['format'])



#-------------------------------------------------------------------------------------------#

