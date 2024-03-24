import subprocess
import os
import vtk

def compile_cpp_executable():
    executable_path = "Parser/stl_parser"
    source_path = "Parser/main.cpp"
    library_path = "Parser/parser.cpp"

    # Check if the executable exists
    if not os.path.exists(executable_path):
        print("C++ executable not found. Compiling...")

        # Compile the C++ source code
        compile_command = ["g++", "-o", executable_path, source_path, library_path, "-std=c++20"]
        result = subprocess.run(compile_command, capture_output=True, text=True)

        # Check if the compilation was successful
        if result.returncode == 0:
            print("Compilation successful.")
        else:
            print("Error during compilation:\n", result.stderr)
            # Exit the script if compilation fails
            exit(1)
    else:
        print("C++ executable found. No compilation needed.")

def parse_stl_with_cpp(stl_path):
    result = subprocess.run(['Parser/stl_parser', stl_path], capture_output=True, text=True)
    if result.returncode != 0:
        print("Error parsing STL file with C++ script.")
        return []
    # Process output to extract vertices...
    vertices_str = result.stdout.split('\n')
    vertices = [list(map(float, v.split())) for v in vertices_str if v]
    return vertices

def visualize_mesh(stl_path, problematic_vertices):
    reader = vtk.vtkSTLReader()
    reader.SetFileName(stl_path)
    # Needed to get the bounds
    reader.Update()

    # Calculate model size to scale sphere sizes
    bounds = reader.GetOutput().GetBounds()
    model_size = max(bounds[1]-bounds[0], bounds[3]-bounds[2], bounds[5]-bounds[4])
    # Sphere radius is 0.5% of the model size
    sphere_radius = model_size * 0.005

    # Create a mapper and actor for the STL model
    mapper = vtk.vtkPolyDataMapper()
    mapper.SetInputConnection(reader.GetOutputPort())
    actor = vtk.vtkActor()
    actor.SetMapper(mapper)

    # Renderer and render window setup
    renderer = vtk.vtkRenderer()
    renderWindow = vtk.vtkRenderWindow()
    renderWindow.AddRenderer(renderer)
    renderWindowInteractor = vtk.vtkRenderWindowInteractor()
    renderWindowInteractor.SetRenderWindow(renderWindow)

    # Add the model actor
    renderer.AddActor(actor)

    # Highlight problematic vertices with spheres
    for vertex in problematic_vertices:
        sphereSource = vtk.vtkSphereSource()
        sphereSource.SetCenter(vertex[0], vertex[1], vertex[2])
        sphereSource.SetRadius(sphere_radius)
        sphereSource.Update()

        # Mapper for the sphere
        sphereMapper = vtk.vtkPolyDataMapper()
        sphereMapper.SetInputConnection(sphereSource.GetOutputPort())

        # Actor for the sphere
        sphereActor = vtk.vtkActor()
        sphereActor.SetMapper(sphereMapper)
        # Red color to highlight problematic vertices
        sphereActor.GetProperty().SetColor(1.0, 0.0, 0.0)

        # Add sphere actor to the renderer
        renderer.AddActor(sphereActor)

    # Background color and rendering
    renderer.SetBackground(.3, .2, .1)
    renderWindow.Render()
    renderWindowInteractor.Start()

def main():
    # Compile C++ executable if it does not exist
    compile_cpp_executable()

    stl_path = input("Enter the path to the STL file (relative to the app.py): ")
    if not os.path.isfile(stl_path):
        print("File does not exist.")
        return

    problematic_vertices = parse_stl_with_cpp(stl_path)
    #print(problematic_vertices)
    visualize_mesh(stl_path, problematic_vertices)

if __name__ == "__main__":
    main()
