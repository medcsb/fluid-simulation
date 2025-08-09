import numpy as np
import matplotlib.pyplot as plt
from matplotlib.widgets import Button, TextBox
from mpl_toolkits.mplot3d import Axes3D

class SPHGridVisualizer:
    def __init__(self, grid_size_x=10, grid_size_y=10, grid_size_z=10):
        self.GRID_SIZE_X = grid_size_x
        self.GRID_SIZE_Y = grid_size_y
        self.GRID_SIZE_Z = grid_size_z
        self.GRID_CENTER = np.array([0.0, 0.0, 0.0])
        
        # Current selected cell
        self.selected_x = 5
        self.selected_y = 5
        self.selected_z = 5
        
        self.setup_plot()
        
    def flat_index_from_grid(self, x, y, z):
        """Convert 3D grid coordinates to flat index"""
        return x + y * self.GRID_SIZE_X + z * self.GRID_SIZE_X * self.GRID_SIZE_Y
    
    def grid_index_from_flat(self, flat_index):
        """Convert flat index back to 3D grid coordinates"""
        z = flat_index // (self.GRID_SIZE_X * self.GRID_SIZE_Y)
        y = (flat_index % (self.GRID_SIZE_X * self.GRID_SIZE_Y)) // self.GRID_SIZE_X
        x = flat_index % self.GRID_SIZE_X
        return x, y, z
    
    def is_valid_grid_index(self, x, y, z):
        """Check if grid coordinates are valid"""
        return (0 <= x < self.GRID_SIZE_X and 
                0 <= y < self.GRID_SIZE_Y and 
                0 <= z < self.GRID_SIZE_Z)
    
    def get_neighbour_indices(self, x, y, z):
        """Get all 27 neighbor indices (including the cell itself)"""
        indices = []
        for dz in [-1, 0, 1]:
            for dy in [-1, 0, 1]:
                for dx in [-1, 0, 1]:
                    nx = x + dx
                    ny = y + dy
                    nz = z + dz
                    
                    if self.is_valid_grid_index(nx, ny, nz):
                        indices.append(self.flat_index_from_grid(nx, ny, nz))
                    else:
                        indices.append(None)  # Invalid neighbor
        return indices
    
    def setup_plot(self):
        """Initialize the 3D plot and controls"""
        self.fig = plt.figure(figsize=(15, 10))
        
        # Main 3D plot
        self.ax = self.fig.add_subplot(121, projection='3d')
        
        # Info panel
        self.info_ax = self.fig.add_subplot(122)
        self.info_ax.axis('off')
        
        # Control buttons and text boxes
        ax_x = plt.axes([0.1, 0.02, 0.1, 0.04])
        ax_y = plt.axes([0.25, 0.02, 0.1, 0.04])
        ax_z = plt.axes([0.4, 0.02, 0.1, 0.04])
        ax_update = plt.axes([0.55, 0.02, 0.1, 0.04])
        ax_prev = plt.axes([0.7, 0.02, 0.08, 0.04])
        ax_next = plt.axes([0.8, 0.02, 0.08, 0.04])
        
        self.text_x = TextBox(ax_x, 'X: ', initial=str(self.selected_x))
        self.text_y = TextBox(ax_y, 'Y: ', initial=str(self.selected_y))
        self.text_z = TextBox(ax_z, 'Z: ', initial=str(self.selected_z))
        self.btn_update = Button(ax_update, 'Update')
        self.btn_prev = Button(ax_prev, 'Previous')
        self.btn_next = Button(ax_next, 'Next')
        
        # Connect events
        self.btn_update.on_clicked(self.update_selection)
        self.btn_prev.on_clicked(self.previous_cell)
        self.btn_next.on_clicked(self.next_cell)
        
        self.update_visualization()
    
    def update_selection(self, event=None):
        """Update selection based on text box input"""
        try:
            x = int(self.text_x.text)
            y = int(self.text_y.text)
            z = int(self.text_z.text)
            
            if self.is_valid_grid_index(x, y, z):
                self.selected_x = x
                self.selected_y = y
                self.selected_z = z
                self.update_visualization()
            else:
                print(f"Invalid coordinates: ({x}, {y}, {z})")
        except ValueError:
            print("Please enter valid integers")
    
    def previous_cell(self, event):
        """Go to previous cell in flat index order"""
        current_flat = self.flat_index_from_grid(self.selected_x, self.selected_y, self.selected_z)
        if current_flat > 0:
            new_flat = current_flat - 1
            x, y, z = self.grid_index_from_flat(new_flat)
            self.selected_x, self.selected_y, self.selected_z = x, y, z
            self.text_x.set_val(str(x))
            self.text_y.set_val(str(y))
            self.text_z.set_val(str(z))
            self.update_visualization()
    
    def next_cell(self, event):
        """Go to next cell in flat index order"""
        current_flat = self.flat_index_from_grid(self.selected_x, self.selected_y, self.selected_z)
        max_flat = self.GRID_SIZE_X * self.GRID_SIZE_Y * self.GRID_SIZE_Z - 1
        if current_flat < max_flat:
            new_flat = current_flat + 1
            x, y, z = self.grid_index_from_flat(new_flat)
            self.selected_x, self.selected_y, self.selected_z = x, y, z
            self.text_x.set_val(str(x))
            self.text_y.set_val(str(y))
            self.text_z.set_val(str(z))
            self.update_visualization()
    
    def update_visualization(self):
        """Update the 3D visualization"""
        self.ax.clear()
        
        # Get neighbors of selected cell
        neighbors = self.get_neighbour_indices(self.selected_x, self.selected_y, self.selected_z)
        neighbor_coords = set()
        
        for neighbor_flat in neighbors:
            if neighbor_flat is not None:
                nx, ny, nz = self.grid_index_from_flat(neighbor_flat)
                neighbor_coords.add((nx, ny, nz))
        
        # Create grid visualization
        x_coords, y_coords, z_coords = [], [], []
        colors = []
        
        for x in range(self.GRID_SIZE_X):
            for y in range(self.GRID_SIZE_Y):
                for z in range(self.GRID_SIZE_Z):
                    x_coords.append(x)
                    y_coords.append(y)
                    z_coords.append(z)
                    
                    # Color coding
                    if (x, y, z) == (self.selected_x, self.selected_y, self.selected_z):
                        colors.append('blue')  # Selected cell
                    elif (x, y, z) in neighbor_coords:
                        colors.append('green')  # Neighbors
                    else:
                        colors.append('lightgray')  # Other cells
        
        # Plot all cells
        self.ax.scatter(x_coords, y_coords, z_coords, c=colors, s=20, alpha=0.6)
        
        # Highlight selected cell with larger marker
        self.ax.scatter([self.selected_x], [self.selected_y], [self.selected_z], 
                       c='darkblue', s=100, marker='s')
        
        # Set labels and limits
        self.ax.set_xlabel('X')
        self.ax.set_ylabel('Y')
        self.ax.set_zlabel('Z')
        self.ax.set_xlim(0, self.GRID_SIZE_X-1)
        self.ax.set_ylim(0, self.GRID_SIZE_Y-1)
        self.ax.set_zlim(0, self.GRID_SIZE_Z-1)
        
        title = f'SPH Grid Neighbors - Selected: ({self.selected_x}, {self.selected_y}, {self.selected_z})'
        self.ax.set_title(title)
        
        # Update info panel
        self.update_info_panel(neighbors)
        
        plt.draw()
    
    def update_info_panel(self, neighbors):
        """Update the information panel with neighbor details"""
        self.info_ax.clear()
        self.info_ax.axis('off')
        
        # Selected cell info
        selected_flat = self.flat_index_from_grid(self.selected_x, self.selected_y, self.selected_z)
        
        info_text = f"Selected Cell Information:\n"
        info_text += f"Coordinates: ({self.selected_x}, {self.selected_y}, {self.selected_z})\n"
        info_text += f"Flat Index: {selected_flat}\n\n"
        
        info_text += f"Neighbors (27 total, including invalid):\n"
        info_text += "=" * 40 + "\n"
        
        valid_neighbors = []
        invalid_count = 0
        
        # Organize neighbors by layer (z-1, z, z+1)
        for layer, dz in enumerate([-1, 0, 1]):
            info_text += f"\nZ-Layer {self.selected_z + dz} (dz={dz:+d}):\n"
            
            for dy in [-1, 0, 1]:
                row_neighbors = []
                for dx in [-1, 0, 1]:
                    idx = layer * 9 + (dy + 1) * 3 + (dx + 1)
                    neighbor_flat = neighbors[idx]
                    
                    if neighbor_flat is not None:
                        nx, ny, nz = self.grid_index_from_flat(neighbor_flat)
                        row_neighbors.append(f"({nx},{ny},{nz})")
                        valid_neighbors.append((nx, ny, nz))
                    else:
                        row_neighbors.append("INVALID")
                        invalid_count += 1
                
                info_text += "  " + " | ".join(f"{n:>9}" for n in row_neighbors) + "\n"
        
        info_text += f"\nSummary:\n"
        info_text += f"Valid neighbors: {len(valid_neighbors)}\n"
        info_text += f"Invalid neighbors: {invalid_count}\n"
        info_text += f"Total: {len(neighbors)}\n"
        
        # Color legend
        info_text += f"\nColor Legend:\n"
        info_text += f"🔵 Blue: Selected cell\n"
        info_text += f"🟢 Green: Valid neighbors\n"
        info_text += f"⚪ Gray: Other cells\n"
        
        self.info_ax.text(0.05, 0.95, info_text, transform=self.info_ax.transAxes, 
                         fontfamily='monospace', fontsize=8, verticalalignment='top')
    
    def show(self):
        """Display the visualization"""
        plt.tight_layout()
        plt.show()

# Create and run the visualizer
if __name__ == "__main__":
    # You can adjust grid size here to match your SPH solver
    visualizer = SPHGridVisualizer(grid_size_x=10, grid_size_y=10, grid_size_z=10)
    
    print("SPH Grid Neighbor Visualizer")
    print("=" * 50)
    print("Controls:")
    print("- Enter X, Y, Z coordinates in the text boxes and click 'Update'")
    print("- Use 'Previous'/'Next' buttons to navigate through cells in flat index order")
    print("- Blue cell = Selected cell")
    print("- Green cells = Valid neighbors")
    print("- Gray cells = Other cells")
    print("- Invalid neighbors are not shown (outside grid bounds)")
    
    visualizer.show()