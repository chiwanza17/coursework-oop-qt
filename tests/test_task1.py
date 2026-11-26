"""Run from the repo root after build.bat: python tests/test_task1.py."""

from datetime import date
from pathlib import Path
import re
import subprocess
import tempfile
import unittest


EXE = Path(__file__).resolve().parents[1] / "phonebook.exe"
PHONES = (
    "+78121234567", "88121234567", "+7(812)1234567",
    "8(812)1234567", "+7(812)123-45-67", "8(812)123-45-67",
)


class TelephoneDirectoryTests(unittest.TestCase):
    def setUp(self):
        directory = tempfile.TemporaryDirectory()
        self.addCleanup(directory.cleanup)
        self.directory = Path(directory.name)

    def console(self, *lines):
        result = subprocess.run(
            [str(EXE)], cwd=self.directory,
            input=("\n".join((*lines, ""))).encode("utf-8"),
            capture_output=True, timeout=10,
        )
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertEqual(result.stderr, b"")
        return result.stdout.decode("utf-8")

    def test_phone_formats(self):
        invalid = (
            "", "+18121234567", "78121234567", "+88121234567",
            "+7812123456", "881212345678", "+7812 1234567",
            "+7(81)1234567", "8(8121234567", "8(812)1234-5-67",
            "+7(812)123456-7", "+7812123456x", "8(812)123-45-67 ext 1",
        )
        for index, phone in enumerate(PHONES):
            with self.subTest(phone=phone):
                rejected = invalid if index == 0 else ()
                output = self.console(
                    "1", "Test", "Contact", "", "", "", "test@example.com",
                    *rejected, f" {phone} ", str(index % 3 + 1), "0",
                )
                self.assertEqual(output.count("Invalid phone format."), len(rejected))
                self.assertIn("Contact added.", output)
        output = self.console("2", "0")
        for index, phone in enumerate(PHONES):
            kind = ("Home", "Work", "Service")[index % 3]
            self.assertIn(f"Phone 1 ({kind}): {phone}\r\n", output)

    def test_required_fields_names_email_and_calendar(self):
        invalid_dates = (
            "2020-00-01", "2020-13-01", "2020-01-00", "2020-01-32",
            "2020-04-31", "2019-02-29", "1900-02-29", "0000-01-01",
            "29.02.2000", date.today().isoformat(), "9999-12-31",
        )
        output = self.console(
            "1", "", "2Anna", "-Anna", "Anna-", "Ann_a", " Anna 2-Marie ",
            "", "9Smith", " Петрова ", "-Ивановна", " Ивановна ",
            '  10 "Main" Street  ', *invalid_dates, " 2000-02-29 ",
            "", "missingat", "ann+1@example.com", "ann@bad-domain.com",
            "@example.com", "ann@", " ann1 \t@ example2 . com ",
            PHONES[0], "1", "0",
        )
        self.assertEqual(output.count("Invalid name:"), 8)
        self.assertEqual(output.count("Invalid birth date."), len(invalid_dates))
        self.assertEqual(output.count("Invalid email."), 6)
        self.assertIn("Contact added.", output)
        loaded = self.console("2", "0")
        for field in (
            "Anna 2-Marie Петрова", "Patronymic: Ивановна",
            'Address: 10 "Main" Street', "Birth date: 2000-02-29",
            "Email: ann1@example2.com",
        ):
            self.assertIn(field + "\r\n", loaded)

    def test_edit_all_fields_multiple_phones_and_delete(self):
        self.assertIn("No contacts found.", self.console("2", "0"))
        self.console("1", "Old", "Name", "", "", "", "old@example.com", PHONES[0], "1", "0")
        output = self.console(
            "3", "1",
            "1", "New", "2", "Surname", "3", "Middle", "4", "New address",
            "5", "2004-02-29", "6", "new@example.com",
            "7", "3", "1",
            "2", "1", PHONES[1], "2",
            "1", PHONES[2], "3", "1", PHONES[3], "1",
            "1", PHONES[4], "2", "3", "3", "0", "0", "0",
        )
        self.assertIn("A contact must have at least one phone number.", output)
        loaded = self.console("2", "0")
        for field in (
            "New Surname", "Patronymic: Middle", "Address: New address",
            "Birth date: 2004-02-29", "Email: new@example.com",
            f"Phone 1 (Work): {PHONES[1]}", f"Phone 2 (Service): {PHONES[2]}",
            f"Phone 3 (Work): {PHONES[4]}",
        ):
            self.assertIn(field + "\r\n", loaded)
        for phone in (PHONES[0], PHONES[3]):
            self.assertNotIn(f"): {phone}\r\n", loaded)
        self.console("4", "1", "n", "0")
        self.assertIn("New Surname", self.console("2", "0"))
        self.assertIn("Contact deleted.", self.console("4", "1", "y", "0"))
        self.assertIn("No contacts found.", self.console("2", "0"))

    def test_sort_each_field_and_search_across_fields(self):
        self.console(
            "1", "Zoe", "Adams", "Zulu Middle", "Alpha Street", "2000-01-01",
            "zulu@example.com", "88001234567", "1",
            "1", "Anna", "Zed", "Alpha", "Zulu Lane", "2002-01-01",
            "alpha@example.com", "88121234567", "2", "7", "0",
        )
        for field, first in enumerate(("Anna", "Zoe", "Anna", "Anna", "Zoe", "Zoe", "Zoe"), 1):
            with self.subTest(sort_field=field):
                output = self.console("6", str(field), "0")
                self.assertEqual(re.findall(r"^\[1\] (\w+)", output, re.M), [first])
        for query in ("ZOE", "Adams", "Zulu Middle", "Alpha Street", "2000-01-01", "zulu@example.com", "8800123"):
            with self.subTest(query=query):
                self.assertIn("Zoe Adams", self.console("5", query, "0"))
        self.assertIn("Anna Zed", self.console("5", "Zulu Lane", "0"))
        self.assertIn("No matches.", self.console("5", "unmatched term", "0"))
        self.assertTrue((self.directory / "data" / "contacts.txt").is_file())


if __name__ == "__main__":
    unittest.main(verbosity=2)
