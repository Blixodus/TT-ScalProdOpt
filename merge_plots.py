import os
from pypdf import PdfWriter, PdfReader, PageObject

def merge_side_by_side(pdf1_path, pdf2_path, output_path):
    print("Merging: ", pdf1_path, pdf2_path)
    
    input1 = PdfReader(open(pdf1_path, "rb"), strict=False)
    input2 = PdfReader(open(pdf2_path, "rb"), strict=False)

    page1 = input1.get_page(0)
    page2 = input2.get_page(0)

    total_width = page1.mediabox.right + page2.mediabox.right
    total_height = max([page1.mediabox.top, page2.mediabox.top])

    new_page = PageObject.create_blank_page(None, total_width, total_height)

    # Add first page at the 0,0 position
    new_page.merge_page(page1)
    # Add second page with moving along the axis x
    new_page.merge_translated_page(page2, page1.mediabox.right, 0)

    output = PdfWriter()
    output.add_page(new_page)
    output.write(open(output_path, "wb"))

def traverse_and_merge(dir1, dir2, output_dir):
    for root, _, files in os.walk(dir1):
        for file in files:
            if file.endswith('.pdf'):
                pdf1_path = os.path.join(root, file)
                relative_path = os.path.relpath(pdf1_path, dir1)
                pdf2_path = os.path.join(dir2, relative_path)

                if os.path.exists(pdf2_path):
                    # Ensure the output directory exists
                    output_subdir = os.path.join(output_dir, os.path.dirname(relative_path))
                    os.makedirs(output_subdir, exist_ok=True)

                    output_path = os.path.join(output_dir, relative_path)
                    merge_side_by_side(pdf1_path, pdf2_path, output_path)

# Example usage
traverse_and_merge("/gpfs/workdir/torria/pdominik/Plots_review/Plots", "/gpfs/workdir/torria/pdominik/Plots_review/Plots_old", "/gpfs/workdir/torria/pdominik/Plots_review/Plots_merged")