## ✅ How to Run (Visual Studio)

1. Download and unzip the project folder.
2. Open `CGassignment6.sln` in **Visual Studio**.
3. Set build configuration:
   - **Configuration**: `Debug`
   - **Platform**: `x86`
4. Right-click the desired project (e.g., `Q2`) in the **Solution Explorer** and choose  
   **"Set as Startup Project"**
5. Press **F5** to build and run.

---

## 🔧 Required Project Settings

> These should already be set in the `.vcxproj` file, but double-check if needed.

- **C++ Language Standard**  
  `Project → Properties → C/C++ → Language → C++ Language Standard`  
  → **`ISO C++17 Standard (/std:c++17)`**

- **Include Directory**  
  `Project → Properties → C/C++ → General → Additional Include Directories`  
  → `$(ProjectDir)include`

- **Library Directory**  
  `Project → Properties → Linker → General → Additional Library Directories`  
  → `$(ProjectDir)lib`

- **Linker Input**  
  `Project → Properties → Linker → Input → Additional Dependencies`  
  → `freeglut.lib`

✅ No external installation required  
✅ All necessary libraries are included in the project folder

---

## 📸 Screenshot Results

Below are the rendered results for each shading method:

### Q1 – Flat Shading  
![Q1 result screenshot](https://github.com/user-attachments/assets/b93799f2-c06a-4fd6-b7fa-e318ae69302e)


---

### Q2 – Gouraud Shading  
![Q2 result screenshot](https://github.com/user-attachments/assets/540257c9-bcf7-41b6-a5a6-0823141638f4)


---

### Q3 – Phong Shading  
![Q3 result screenshot](https://github.com/user-attachments/assets/7ed14228-78d9-4881-9587-a52fdf98d2bd)


---
