import subprocess

def generate_gstreamer_pipeline(width, height,formate):
    # Construct GStreamer pipeline command based on the configuration
    pipeline_cmd = (
        f'gst-launch-1.0 videotestsrc ! '
        f'video/x-raw,width={width},height={height},format={formate} ! '
        f'autovideosink'
    )
    return pipeline_cmd

def read_lst_file(file_path):
    # Read configurations from the lst file
    with open(file_path, 'r') as file:
        lines = file.readlines()

    # Parse configurations from each line
    configurations = []
    for line in lines:
        values = line.strip().split(',')
        print("values:",values)
        if len(values) == 3:
            try:
                width, height = map(int, values[:2])
                formate = values[2]
                configurations.append({'width': width, 'height': height,'format':formate})
            except ValueError:
                print(f"Skipping invalid line: {line}")

    return configurations

def run_gstreamer_pipeline(width, height,formate):
    pipeline_cmd = generate_gstreamer_pipeline(width, height,formate)

    # Run the GStreamer pipeline
    try:
        subprocess.run(pipeline_cmd, shell=True, check=True)
        print(f'GStreamer pipeline {width}x{height} completed successfully.')
    except subprocess.CalledProcessError as e:
        print(f'Error running GStreamer pipeline {width}x{height}: {e}')

if __name__ == "__main__":
    lst_file_path = "/home/bhargav/Downloads/Scripts/With_LST/my_test.lst"
    configurations = read_lst_file(lst_file_path)
    print("config",configurations)

    # Run GStreamer pipelines based on configurations
    for config in configurations:
        run_gstreamer_pipeline(config['width'], config['height'],config['format'])

