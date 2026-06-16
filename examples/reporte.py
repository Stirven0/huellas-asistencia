import pandas as pd
import sys
from pathlib import Path

DIR = Path(__file__).parent

alumnos = pd.read_csv(DIR / "ALUMNOS.CSV", dtype=str, keep_default_na=False)
alumnos["NombreCompleto"] = alumnos["Nombre"] + " " + alumnos["Apellido"]

asis = pd.read_csv(DIR / "ASIST.CSV", dtype=str, keep_default_na=False)
asis["Presente"] = 1

pivot = asis.pivot_table(
    index="ID", columns="Fecha", values="Presente",
    aggfunc="first"
).fillna(0).astype(int)

tabla = pivot.merge(
    alumnos[["ID", "NombreCompleto"]], on="ID", how="left"
).set_index(["ID", "NombreCompleto"])

cols = sorted(tabla.columns)
tabla = tabla[cols]
tabla["Total"] = tabla.sum(axis=1)

if "--csv" in sys.argv:
    out = DIR / "reporte_asistencia.csv"
    tabla.to_csv(out)
    print(f"Guardado en {out}")
else:
    pd.set_option("display.max_columns", 50)
    pd.set_option("display.width", 200)
    pd.set_option("display.max_rows", 50)
    print(tabla.to_string())
