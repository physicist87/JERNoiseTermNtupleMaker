import os
import re
import sys
import time
import argparse
import subprocess
from datetime import datetime, timedelta

class CrabTask:
    def __init__(self, project_dir):
        self.project_dir = project_dir
        self.task_name = os.path.basename(project_dir.rstrip('/'))
        self.is_complete = False

    def _run_crab_cmd(self, action_cmd):
        """
        Helper method to run CRAB commands after strictly setting up 'cmsenv'.
        Uses the exact current working directory to evaluate CMSSW correctly.
        """
        # Execute cmsenv before running any CRAB actions to safeguard the CMSSW environment
        full_cmd = f"eval `scramv1 runtime -sh` && {action_cmd}"
        current_env = os.environ.copy()

        result = subprocess.run(
            full_cmd,
            shell=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            env=current_env
        )
        return result

    def check_status(self):
        """
        Executes 'crab status' under the local CMSSW environment.
        Parses exact job status metrics and determines completion.
        """
        print(f"\n[INFO] Checking status for: {self.task_name}")

        cmd = f"crab status -d {self.project_dir}"
        result = self._run_crab_cmd(cmd)

        # Catch hard shell script failures or explicit missing CMSSW errors
        if result.returncode != 0 or "CMSSW is missing" in result.stderr or "CMSSW is missing" in result.stdout:
            print(f"  [ERROR] Failed to execute 'crab status' command properly.")
            print(f"  [Details] {result.stderr.strip() if result.stderr.strip() else result.stdout.strip()}")
            return False

        output = result.stdout + "\n" + result.stderr
        job_stats = {}

        for line in output.split('\n'):
            if '(' in line and '/' in line:
                state_match = re.search(r'(finished|failed|running|idle|transferring|unsubmitted|cooloff)', line)
                count_match = re.search(r"\(\s*(\d+)\s*/\s*(\d+)\s*\)", line)

                if state_match and count_match:
                    state_name = state_match.group(1)
                    num = int(count_match.group(1))
                    denom = int(count_match.group(2))
                    job_stats[state_name] = (num, denom)

        if job_stats:
            print("  [JOB METRICS SUMMARY]")
            total_jobs = list(job_stats.values())[0][1]
            finished_jobs = job_stats.get('finished', (0, total_jobs))[0]

            for state, (num, denom) in job_stats.items():
                print(f"    - {state.ljust(13)}: {num}/{denom}")

            if finished_jobs == total_jobs and total_jobs > 0:
                print("  [STATUS] 100% Completed. No resubmission required.")
                self.is_complete = True
                return True
            else:
                remaining = total_jobs - finished_jobs
                print(f"  [STATUS] Incomplete. {remaining} jobs are not 'finished' yet.")
                self.is_complete = False
                return False
        else:
            print("  [ERROR] Could not parse job status lines. The task might be initializing or broken.")
            print("  [DEBUG] Command response preview (First 8 lines):")
            lines = [l.strip() for l in output.split('\n') if l.strip()]
            for l in lines[:8]:
                print(f"    > {l}")
            return False

    def resubmit(self):
        """
        Executes 'crab resubmit' safely after setting up 'cmsenv'.
        """
        print(f"  [ACTION] Resubmitting task: {self.task_name} ...")
        cmd = f"crab resubmit -d {self.project_dir}"
        result = self._run_crab_cmd(cmd)

        # Verify if the resubmission was truly accepted by the CRAB server
        if result.returncode == 0 and "CMSSW is missing" not in result.stdout and "failed" not in result.stdout.lower():
            print("  [SUCCESS] Resubmit command accepted by the server.")
        else:
            print(f"  [ERROR] Resubmit action failed completely.")
            details = result.stderr.strip() if result.stderr.strip() else result.stdout.strip()
            print(f"  [Details] {details[:200]}")


class CrabManager:
    def __init__(self, base_path, interval_hours, total_hours):
        self.base_path = base_path
        self.interval_seconds = interval_hours * 3600
        self.total_seconds = total_hours * 3600
        self.tasks = []

    def discover_tasks(self):
        """
        Scans the target path recursively for valid directories starting with 'crab_'.
        Excludes specific backup paths like 'ZeroBias_Run2025_bk'.
        """
        self.tasks = []
        search_dir = self.base_path

        if not os.path.exists(search_dir):
            print(f"[FATAL] Target directory does not exist: {search_dir}")
            sys.exit(1)

        # Recursively walk through directories
        for root, dirs, files in os.walk(search_dir):
            # 1. Skip the backup folder to avoid scanning unwanted tasks
            if 'ZeroBias_Run2025_bk' in dirs:
                dirs.remove('ZeroBias_Run2025_bk')
            
            # 2. Identify CRAB task directories starting with 'crab_'
            for d in dirs:
                if d.startswith('crab_'):
                    full_path = os.path.join(root, d)
                    self.tasks.append(CrabTask(full_path))

        print(f"[INFO] Discovered {len(self.tasks)} CRAB tasks in {search_dir}")
        for t in self.tasks:
            print(f"   -> Found: {t.project_dir}")

    def run_loop(self):
        """
        Main runner maintaining scheduling loops and tracking tasks.
        """
        start_time = datetime.now()
        end_time = start_time + timedelta(seconds=self.total_seconds)

        print(f"[START] Monitoring process initiated at {start_time.strftime('%Y-%m-%d %H:%M:%S')}")
        print(f"[INFO] Total runtime limit: {self.total_seconds/3600:.1f} hours (Ends at: {end_time.strftime('%Y-%m-%d %H:%M:%S')})")
        print(f"[INFO] Checking interval: {self.interval_seconds/3600:.1f} hour(s)\n")

        while datetime.now() < end_time:
            current_run_start = datetime.now()
            print(f"\n{'='*60}\n[LOOP START] {current_run_start.strftime('%Y-%m-%d %H:%M:%S')}\n{'='*60}")

            self.discover_tasks()

            if len(self.tasks) == 0:
                print("[WARNING] No CRAB tasks found in this turn. Will retry in the next interval.")

            active_tasks_count = 0
            for task in self.tasks:
                is_finished = task.check_status()
                if not is_finished:
                    task.resubmit()
                    active_tasks_count += 1

            if len(self.tasks) > 0 and active_tasks_count == 0:
                print("\n[SUCCESS] All discovered CRAB tasks are fully COMPLETED! Exiting manager.")
                break

            elapsed = (datetime.now() - current_run_start).total_seconds()
            sleep_time = max(0, self.interval_seconds - elapsed)

            next_run = datetime.now() + timedelta(seconds=sleep_time)
            if next_run > end_time:
                print("\n[END] Next interval exceeds the remaining time window. Stopping manager.")
                break

            print(f"\n[WAIT] Next check scheduled at {next_run.strftime('%Y-%m-%d %H:%M:%S')} (Sleeping for {sleep_time/3600:.2f} hours...)")
            time.sleep(sleep_time)

        print(f"\n[FINISHED] Automation script terminated at {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="CMS CRAB Automated Resubmitter (OOP Architecture)")
    parser.add_argument('--path', type=str, default="crab", help="Base directory containing CRAB outputs (e.g., 'crab')")
    parser.add_argument('--interval', type=float, default=1.0, help="Interval between checks in hours (default: 1.0)")
    parser.add_argument('--total', type=float, default=24.0, help="Total execution duration in hours (default: 24.0)")

    args = parser.parse_args()

    manager = CrabManager(base_path=args.path, interval_hours=args.interval, total_hours=args.total)
    manager.run_loop()
