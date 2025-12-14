import os
import torch
import torch_mlir
from torch_mlir import fx as torch_mlir_fx

# Disable CUDA for the export phase to avoid runtime device conflicts during compilation
os.environ["CUDA_VISIBLE_DEVICES"] = ""

class TinyMLP(torch.nn.Module):
    def __init__(self):
        super().__init__()
        self.fc1 = torch.nn.Linear(4, 8)
        self.relu = torch.nn.ReLU()
        self.fc2 = torch.nn.Linear(8, 2)

    def forward(self, x):
        return self.fc2(self.relu(self.fc1(x)))

# Prepare model and input
model = TinyMLP().eval()
example_input = torch.randn(1, 4)

print("Compiling model to Torch MLIR...")

# Export to MLIR
mlir_module = torch_mlir_fx.export_and_import(model, example_input)
mlir_text = mlir_module.operation.get_asm()

# Save to file
output_file = "tiny_mlp.mlir"
with open(output_file, "w") as f:
    f.write(mlir_text)

print(f"Success! MLIR saved to {output_file}")